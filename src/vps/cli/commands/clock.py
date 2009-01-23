#!/usr/bin/python -tt
#
# @(#)clock.py	
#
# Copyright (c) 2008 VirtualPlane, Inc.
#

"""Get/set system date and time - use "clock" or "show clock".
See also "add ntp", "show ntp", and "no ntp".
"""

import getopt
import os
import os.path
import sys
import time


path = 'src/vps/cli'
if path not in sys.path:
     sys.path.append(path)

import lib.output
#import lib.network_config
import lib.process

from lib.errorhandler import *


_TIME_FORMAT             = "%Y %m %B %d %A %H %M %S %z %Z"
_SET_DATE_CMD            = "/bin/date"

_SET_TIMEZONE_ARG        = "timezone"

_XMLTAG_TIMESPEC         = "TimeSpec"

_XMLTAG_Year             = "Year"
_XMLTAG_Month            = "Month"
_XMLTAG_MonthName        = "MonthName"
_XMLTAG_Date             = "Date"
_XMLTAG_DayName          = "DayName"
_XMLTAG_Hour             = "Hour"
_XMLTAG_Minute           = "Minute"
_XMLTAG_Second           = "Second"

_XMLTAG_TZD              = "TZD"
_XMLTAG_TZDoffset        = "TZDoffset"
_XMLTAG_TZDhours         = "TZDhours"
_XMLTAG_TZDminutes       = "TZDminutes"
_XMLTAG_TZDlabel         = "TZDlabel"
_XMLTAG_TZDlocation      = "TZDlocation"

_XMLTAG_TimeZoneList    = "TimeZoneList"
_XMLTAG_TimeZone        = "Timezone"

_TZLOCATION_LINK         = "/etc/localtime"
_TZLOCATION_UNAVAILABLE  = "Unavailable"
_TZ_TARGET_DIR           = "/usr/share/zoneinfo/"


def _get_timezones_in_dir(curSubdir, zones):
    """Search current directory and subdirectory to get the zones
    """
    fulldir = _TZ_TARGET_DIR + "/%s" % curSubdir
    files = os.listdir(fulldir)
    for file in files:
        if os.path.isdir(fulldir + "/" + file):
            _get_timezones_in_dir(curSubdir + "/" + file, zones)
        else: 
            zones.append(curSubdir + '/' + file)
    

def _get_all_timezones():
    """Return the list of all timezone info.
    """
    zones = []
    excludes = ["posix", "posixrules", "right", "zone.tab", "iso3166.tab"]
    files = os.listdir(_TZ_TARGET_DIR)
    for file in files:
        if not file in excludes:
            if os.path.isdir(_TZ_TARGET_DIR + "/" + file):
                # a directory
                _get_timezones_in_dir(file, zones)
            else: # it is a file. 
                zones.append(file)
    zones.sort()
    return zones


def _get_date_format(date):
    """Return the format string for a valid date, or raise ValueError if
    invalid. 
    """
    fmt = "%Y-%m-%d"
    try:
        time.strptime(date, fmt)
    except ValueError:
        raise ValueError('invalid date "%s"' % (date,))
    return fmt


def _get_time_format(time_str):
    """Return the format string for the specified time. 
       Determine whether the optional seconds is included, and adjust the
       format string. Raise ValueError if the supplied time is bad. 
    """

    try:
        fmt = "%H:%M:%S"
        time.strptime(time_str, fmt)
        return fmt
    except ValueError:
        fmt = "%H:%M"
        try:
            time.strptime(time_str, fmt)
        except ValueError:
            raise ValueError('invalid time "%s"' % (time_str,))
        return fmt


def _reformat_timezone(timezone):
    """Gets a timezone string (e.g. "+04:00", "-11:00", or "Z")
       Returns the reformatted timezone, with the ':' removed, and a timezone
       format string to pass to /bin/date. 
       If timezone is 'Z', "+0000" is returned. 
       Raises KeyError if invalid. 
    """

    timezone_fmt = "%z"

    # "Z" = no offset: +0000
    if timezone == "Z":
        return (timezone_fmt, "+0000")

    if timezone == "" or timezone[0] not in '+-':
        raise ValueError("timezone designator must start with '+' or '-'")

    try:
        time.strptime(timezone[1:], "%H:%M")
    except ValueError:
        raise ValueError("bad timezone designator " + timezone)

    timezone_str = timezone.replace(':', '')
    return (timezone_fmt, timezone_str)


def _find_case_insensitive_timezone(timezone):
    zones = _get_all_timezones()
    for zone in zones:
        if zone.upper() == timezone.upper():
            return zone
    return None


def _set_timezone(entered_time_zone):
    output = lib.output.CLIoutput("clock")

    new_time_zone = entered_time_zone
    tz_link_target = _TZ_TARGET_DIR + new_time_zone

    if not os.path.isfile(tz_link_target):
        new_time_zone = _find_case_insensitive_timezone(entered_time_zone)
        if not new_time_zone:
            print NameError('Invalid timezone specified.'
                                   ' Try "show clock timezones [all]".');
            return output
        tz_link_target = _TZ_TARGET_DIR + new_time_zone
        if not os.path.isfile(tz_link_target):
            print NameError("Unable to find timezone file.")
            return output
        
    # Create the symlink at a temporary location, and move it onto the
    # old file (the rename is atomic). 
    tz_link_source_tmp = _TZLOCATION_LINK + ".tmp"
    try :
        # Try to remove the temporary file. If it doesn't exist, just proceed.
        try :
            os.unlink(tz_link_source_tmp)
        except OSError:
            pass
        # Make the link value a relative path from /etc to /usr (..)
        os.symlink(".." + tz_link_target, tz_link_source_tmp)
        os.rename(tz_link_source_tmp, _TZLOCATION_LINK)
    except OSError:
        print NameError("Could not set timezone link.")
        return output
    
    # Set the TZ environment variable to the new timezone datafile
    os.environ['TZ'] = new_time_zone
   
    output.completeOutputSuccess()
    return output



def edit(argv):
    """Set the current time and date
        syntax: [edit] clock <time-spec>
        syntax: [edit] clock timezone <timezone-location>

            The form of the time-spec is
                [<YYYY>-<MM>-<DD>] <HH>:<MM>[:<SS>] [<time-zone-designator>]
            where the time-zone-designator is
                the letter Z or <sign><hh>:<mm> (sign is either '+' or '-')

            The form of the timezone-location is a path such as US/Eastern
            use show clock timezones to check available timezone names.
    """

    if len(argv) == 3 and argv[1] != '' and \
           _SET_TIMEZONE_ARG.startswith(argv[1].lower()):
        return _set_timezone(argv[2])

    output = lib.output.CLIoutput("clock")

    if len(argv) < 2 or len(argv) > 4:
        lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__)
        return output

    # Build the appropriate format and time string to set the time,
    # depending on what's passed in. 

    # Blank all of the strings initially; will be filled in depending on
    # case. 
    date = ""
    time_str = ""     # don't shadow the time module
    timezone = ""
    date_fmt = ""
    time_fmt = ""
    timezone_fmt = ""
    timezone_str = ""

    if len(argv) == 2:         # Received time only. 
        try:
            time_str = argv[1]
            time_fmt = _get_time_format(time_str)
        except ValueError, ex:
            lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__,
                                                        desc=str(ex))
            return output
    elif len(argv) == 3:       # Received date and time, or time and timezone.
        try:
            date_fmt = _get_date_format(argv[1])  # try date and time
            time_fmt = _get_time_format(argv[2])
            date = argv[1]
            time_str = argv[2]
        except ValueError:
            try:                               # try time and timezone
                time_str = argv[1]
                timezone = argv[2]

                time_fmt = _get_time_format(time_str)
                (timezone_fmt, timezone_str) = _reformat_timezone(timezone)
            except ValueError, ex:
                lib.errorhandler.InvalidArgumentCount( 
                    syntax=edit.__doc__, desc=str(ex))
                return output
    else:                      # Received date, time, and timezone
        try:
            date = argv[1]
            time_str = argv[2]
            timezone = argv[3]

            date_fmt = _get_date_format(date)
            time_fmt = _get_time_format(time_str)
            (timezone_fmt, timezone_str) = _reformat_timezone(timezone)
        except ValueError, ex:
            lib.errorhandler.InvalidArgumentCount(syntax=edit.__doc__,
                                                        desc=str(ex))
            return output

    # Prevent the user from changing the time if NTP is configured (bug 5001)
    net_config = lib.network_config.NetworkConfig()
    ntp_config = net_config.getNTPsettings()
    if ntp_config.isNTPconfigured():
        print NameError("The time cannot be manually adjusted because"
                               " NTP is currently active.")
        return output

    # Create a format string, and a time string to pass to /bin/date. 
    format_str = "+" + " ".join((date_fmt, time_fmt, timezone_fmt)).strip()
    value_str = " ".join((date, time_str, timezone_str)).strip()

    set_command = (_SET_DATE_CMD, "-s", value_str, format_str)
    returnValue, out, err, fullstatus = lib.proc.run_command(set_command)

    if returnValue != 0:
        print NameError("Could not set date")
        return output

    output.completeOutputSuccess()
    return output


def _gen_timezones(output, timezones):
    output.beginList(_XMLTAG_TimeZoneList)
    for x in timezones:
        output.setVirtualNameValue(_XMLTAG_TimeZone, x)
    output.endList(_XMLTAG_TimeZoneList)
    
    output.completeOutputSuccess()
    return output


def _show_timezones(output):
    """Show all available timezones
    """
    try:
        timezones = _get_all_timezones()
    except:
        print NameError("get timezone failed");
        return output
    
    return _gen_timezones(output, timezones)


def _show_usa_timezones(output):
    """Show all timezones in the USA
    """
    timezones = [ \
        "US/Alaska", \
        "US/Aleutian", \
        "US/Arizona", \
        "US/Central", \
        "US/East-Indiana", \
        "US/Eastern", \
        "US/Hawaii", \
        "US/Indiana-Starke",\
        "US/Michigan", \
        "US/Mountain", \
        "US/Pacific", \
        "US/Samoa", \
        ]
    return _gen_timezones(output, timezones)


def _output_timespec(outf, name, timespec):
    try:
        year = timespec[_XMLTAG_Year]
        monthname = timespec[_XMLTAG_MonthName]
        date = timespec[_XMLTAG_Date]
        dayname = timespec[_XMLTAG_DayName]
        hour = timespec[_XMLTAG_Hour]
        minute = timespec[_XMLTAG_Minute]
        second = timespec[_XMLTAG_Second]
        tzd = timespec[_XMLTAG_TZD]
        label = tzd[_XMLTAG_TZDlabel]
        tzdlocation = tzd[_XMLTAG_TZDlocation]
    except:
        outf.write("error: incomplete timespec\n")
        return

    fmt = "%-16s%-32s%s"
    time_str = "%s:%s:%s %s" % (hour, minute, second, label)
    date_str = "%s %s %s %s" % (dayname, monthname, date, year)
    outf.write(fmt % ("Time", "Date", "TimeZone"))
    outf.write("\n")
    outf.write(70 * '-')
    outf.write("\n")
    outf.write(fmt % (time_str, date_str, tzdlocation))
    outf.write("\n")


def _output_timezone_list(outf, name, timezones):
    """
    """
    outf.write("%s" % (25 * '-') + " Available Timezones " + "%s" % (25 * '-'))
    outf.write("\n")
    fmt = "%-29s%-29s%s"
    count = 0
    length = len(timezones)
    while count + 3 < length:
        outf.write(fmt % \
                    (timezones[count][1], timezones[count + 1][1], \
                    timezones[count + 2][1]))
        outf.write("\n")
        count += 3

    tmp = ["", "", ""]
    if count <= length - 1:
        for i in range(length - count):
            tmp[i] = timezones[count][1]
            count += 1
        outf.write(fmt % (tmp[0], tmp[1], tmp[2]))
        outf.write("\n")



_output_handlers = {
                        _XMLTAG_TIMESPEC : _output_timespec,
			_XMLTAG_TimeZoneList : _output_timezone_list
                    }
    
def show(argv):
    """Get the current time and date. 
        syntax: show clock
                show clock timezones [all]
        The default is to show timezones in the USA.
    """

    output = lib.output.CLIoutput("clock", _output_handlers)

    if len(argv) > 3:
        lib.errorhandler.InvalidArgumentCount(syntax=show.__doc__)
        return output

    if len(argv) >= 2 and (argv[1] == '' or \
           not "timezones".startswith(argv[1].lower())):
        lib.errorhandler.InvalidArgumentCount(2, argv[1], show.__doc__)
        return output
        
    if len(argv) == 3 and (argv[2] == '' or \
           not "all".startswith(argv[2].lower())):
        lib.errorhandler.InvalidArgumentCount(3, argv[2], show.__doc__)
        return output
        
    if len(argv) == 3:
        return _show_timezones(output)
    elif len(argv) == 2:
        return _show_usa_timezones(output)

    now_time = time.strftime(_TIME_FORMAT)
    dateInfo = now_time.split(' ')

    output.beginAssembling(_XMLTAG_TIMESPEC)

    output.setVirtualNameValue(_XMLTAG_Year,       dateInfo[0])
    output.setVirtualNameValue(_XMLTAG_Month,      dateInfo[1])
    output.setVirtualNameValue(_XMLTAG_MonthName,  dateInfo[2])
    output.setVirtualNameValue(_XMLTAG_Date,       dateInfo[3])
    output.setVirtualNameValue(_XMLTAG_DayName,    dateInfo[4])
    output.setVirtualNameValue(_XMLTAG_Hour,       dateInfo[5])
    output.setVirtualNameValue(_XMLTAG_Minute,     dateInfo[6])
    output.setVirtualNameValue(_XMLTAG_Second,     dateInfo[7])

    TZoffsetString=dateInfo[8]

    output.beginAssembling(_XMLTAG_TZD)

    output.setVirtualNameValue(_XMLTAG_TZDoffset,  TZoffsetString[0])
    output.setVirtualNameValue(_XMLTAG_TZDhours,   TZoffsetString[1:3])
    output.setVirtualNameValue(_XMLTAG_TZDminutes, TZoffsetString[3:5])
    output.setVirtualNameValue(_XMLTAG_TZDlabel,   dateInfo[9])

    TZlocation = _TZLOCATION_UNAVAILABLE
    try: 
        # factor out most of the relative path in the symbolic link value
        TZlocation = os.readlink(_TZLOCATION_LINK)
        TZlocation = TZlocation.replace(".." + _TZ_TARGET_DIR, "")
    except:
        pass

    output.setVirtualNameValue(_XMLTAG_TZDlocation,   TZlocation)

    output.endAssembling(_XMLTAG_TZD)

    output.endAssembling(_XMLTAG_TIMESPEC)
    output.completeOutputSuccess()
    return output



def _usage():
    print >>sys.stderr, "Usage: clock [-s] | clock <time >"

def _main():
    """This function is invoked when clock.py functions as a stand-alone
    program
    """
    try:
        opts, args = getopt.getopt(sys.argv[1:], "d:ish")
    except getopt.GetoptError:
        # print help information and exit:
        _usage()
        sys.exit(2)
    clib = "edit_clock"
    for o, a in opts:
        if o == "-s":
            clib = "show_clock"
        elif o == "-h":
            _usage()
            sys.exit(0)
        else:
            _usage()
            sys.exit(1)
    args.insert(0, clib);
    if clib == "edit_clock":
        edit(args)
    elif clib == "show_clock":
        show(args)
    else:
        print args
    sys.exit(0)

if __name__ == '__main__':
    _main()

