/*
 * Copyright (c) 2008  VirtualPlane Systems, Inc.
 */ 

#include <string.h>
#include <stdio.h>
#include <config.h>


extern int g_bxm_local;
extern int g_bxm_remote;
extern uint8_t g_bxm_protocol;
extern uint8_t g_bridge_mac[6];
extern uint8_t g_bridge_enc_mac[6];
extern uint8_t g_if_name[10];
extern uint8_t g_wwnn[8];
extern uint8_t g_wwpn[8];
extern int g_loglevel;



/* Read Config File */
int 
read_config( )
{
         
        FILE *fp;
        char buff[100];
        char *ptr = NULL;
        char type[50];
        char value[50];
        int pos = 0;

        if ((fp = fopen("bxm.config", "r"))!=NULL) {

                while(fgets(buff, sizeof(buff), fp) != NULL) {

                       if(strpbrk(buff, "#"))
                               continue;

                       if((ptr = strchr(buff,'='))== NULL)
                                continue;
                               
                       memset(type, '\0',sizeof(type));
                       memset(value, '\0',sizeof(value)); 
                                                
                       pos = ptr - buff;                          
                       memcpy(type, buff, pos);
                       ptr++;
                       memcpy(value,ptr, strlen(buff) - pos);

                       process_config_line(type, value);

                       memset(buff, '\0', sizeof(buff));

                }

               print_and_validate_config();
        }
        else {
                /* For now, hard-code the global config */
                g_loglevel = 3;

        }
} 

/*
 * Parse Type and Vlaue. 
 */
void 
process_config_line(char *type, char *value)
{

        str_trim(type);
        str_trim(value);

        if ( strcmp(type, LOG_LEVEL) == 0)
        {
                g_loglevel = atoi(value);
        }
        else if ( strcmp(type,MGMT_MODE_LOCAL) == 0 ) {
                g_bxm_local = atoi(value);
        }
        else if ( strcmp(type,MGMT_MODE_REMOTE )== 0) {
                g_bxm_remote = atoi(value);
        }
        else if ( strcmp(type,INTERNAL_PROTOCOL) == 0) {
                if (strcmp(value,INTERNAL_PROTOCOL_EN) == 0)
                        g_bxm_protocol = BXM_EN_PROTOCOL;
                else if (strcmp(value,INTERNAL_PROTOCOL_IB) == 0)
                        g_bxm_protocol = BXM_IB_PROTOCOL;
                else if (strcmp(value,INTERNAL_PROTOCOL_FC) == 0)
                        g_bxm_protocol = BXM_FC_PROTOCOL;

        }
        else if ( strcmp(type,GW_INTERNAL_MAC) == 0) {
                parse_address("gw_internal_mac", value, 6, g_bridge_enc_mac);
        }
        else if ( strcmp(type,GW_MAC) == 0) {
                parse_address("gw_mac", value, 6, g_bridge_mac);
        }
        else if ( strcmp(type,VFM_ETH_INTF) == 0) {
                 strcpy (g_if_name,value);
        }
        else if ( strcmp(type,WWPN) == 0) {
                parse_address("wwpn", value, 8, g_wwpn);
        }
        else if ( strcmp(type,WWNN) == 0) {
                parse_address("wwnn", value, 8, g_wwnn);
        }
}

/*
 * Print and Validate configuration.
 */
void 
print_and_validate_config()
{

        if (g_bxm_local == 1) {
                printf("BXM TYPE : Local  BXM \n");
        }
        else if (g_bxm_local > 1 || g_bxm_local < 0) {
                printf("BXM TYPE : Invalid Local BXM type\n");
                abort();
        }

        if (g_bxm_remote == 1) {
                printf("BXM TYPE : Remote  BXM \n");
        }
        else if ((g_bxm_remote > 1) || (g_bxm_local < 0)) {
                printf("BXM TYPE : Invalid  Remote BXM type\n");
                abort();
        }

        if (g_bxm_remote == 1 &&  g_bxm_remote == 1) {
                printf("BXM TYPE's : Local and remote BXM\n");
        }

        if (g_bxm_protocol == BXM_EN_PROTOCOL ) {
                printf("BXM Protocol Type: EN\n");
        }
        else if (g_bxm_protocol == BXM_IB_PROTOCOL) {
                printf("BXM Protocol Type: IB \n");
        }
        else if (g_bxm_protocol == BXM_FC_PROTOCOL) {
                printf("BXM Protocol Type: FC \n");
        }
        else {
                printf("BXM Protocol Type: Unknown \n");
                abort();
        }
        
        printf("BXM Interface Name : %s\n", g_if_name);

        print_address("Bridge Internal mac",g_bridge_enc_mac, 6);
        print_address("Bridge mac",g_bridge_mac, 6);
        print_address("WWNN",g_wwnn, 8);
        print_address("WWPN",g_wwpn, 8);


        printf("Log Level  : %d \n", g_loglevel);

}


/* Print Mac/WWPN/WWNN address */
void 
print_address(const char *name, uint8_t *addr, uint8_t length)
{
        int i = 0;

        printf("%s: ",name);

        for (i = 0; i<length; i++) {
                printf("%0.2X", addr[i]);
                if(i < (length - 1))
                        printf(":");
        }
        printf("\n");
}


/* String Trim */
void
str_trim(char *str)
{
        int i = 0, j = 0;
        char temp[30];

        for (i = 0; i < strlen(str); i++) {

                if(str[i]!= ' ' && str[i]!='\t' && str[i]!= '\n'){
                        temp[j++] = str[i];
                }
        }
        memset(str, '\0', strlen(str));
        memcpy(str, temp, j);
}

/*
 * Parse input buffer to address according to address length.
 * Mac or WWPN or WWNN address.
 *
 * [IN] input_type : Name of address holder.
 * [IN] *buff      : Input buufer read from the file (Only one line).\
 * [IN] length     : Length of address.(Mac = 6, WWPN and WWNN length=8).
 * [OUT]*out_buff  : Output address buffer.
 */
void
parse_address(const char *input_type,
                          uint8_t *buff,
                          int length,
                          uint8_t *out_buff)
{
        int i =0, j=0;
        for (i =0; i < length*3-1; i++) {
                if (buff[i] == ':')
                        continue;
                else if (buff[i] >= 'A' && buff[i] <= 'F')
                        buff[i] -= 55;
                else if (buff[i] >= 'a' && buff[i] <= 'f')
                        buff[i] -= 87;
                else if (buff[i] >= '0' && buff[i] <= '9')
                        buff[i] -= 48;
                else {
                        printf("ERROR:Incorrect (%s) specified\n", input_type);
                        abort();
                }
        }
        for (i =0; i < length; i++, j+=3)
                out_buff[i] = buff[j]<<4 | buff[j+1];
}
