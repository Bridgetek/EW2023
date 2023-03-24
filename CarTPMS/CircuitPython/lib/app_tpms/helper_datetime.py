"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com
"""

import time
from .helper_math import *
import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve

class _evehp_datetime_glob:
    '''Internal data'''

    timezone = [
        "(UTC-12:00) International Date Line West                      ",
        "(UTC-11:00) Coordinated Universal Time-11                     ",
        "(UTC-10:00) Hawaii                                            ",
        "(UTC-09:00) Alaska                                            ",
        "(UTC-08:00) Baja California                                   ",
        "(UTC-08:00) Pacific Time (US and Canada)                      ",
        "(UTC-07:00) Chihuahua, La Paz, Mazatlan                       ",
        "(UTC-07:00) Arizona                                           ",
        "(UTC-07:00) Mountain Time (US and Canada)                     ",
        "(UTC-06:00) Central America                                   ",
        "(UTC-06:00) Central Time (US and Canada)                      ",
        "(UTC-06:00) Saskatchewan                                      ",
        "(UTC-06:00) Guadalajara, Mexico City, Monterey                ",
        "(UTC-05:00) Bogota, Lima, Quito                               ",
        "(UTC-05:00) Indiana (East)                                    ",
        "(UTC-05:00) Eastern Time (US and Canada)                      ",
        "(UTC-04:30) Caracas                                           ",
        "(UTC-04:00) Atlantic Time (Canada)                            ",
        "(UTC-04:00) Asuncion                                          ",
        "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan              ",
        "(UTC-04:00) Cuiaba                                            ",
        "(UTC-04:00) Santiago                                          ",
        "(UTC-03:30) Newfoundland                                      ",
        "(UTC-03:00) Brasilia                                          ",
        "(UTC-03:00) Greenland                                         ",
        "(UTC-03:00) Cayenne, Fortaleza                                ",
        "(UTC-03:00) Buenos Aires                                      ",
        "(UTC-03:00) Montevideo                                        ",
        "(UTC-02:00) Coordinated Universal Time-2                      ",
        "(UTC-01:00) Cape Verde                                        ",
        "(UTC-01:00) Azores                                            ",
        "(UTC+00:00) Casablanca                                        ",
        "(UTC+00:00) Monrovia, Reykjavik                               ",
        "(UTC+00:00) Dublin, Edinburgh, Lisbon, London                 ",
        "(UTC+00:00) Coordinated Universal Time                        ",
        "(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna  ",
        "(UTC+01:00) Brussels, Copenhagen, Madrid, Paris               ",
        "(UTC+01:00) West Central Africa                               ",
        "(UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague ",
        "(UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb                  ",
        "(UTC+01:00) Windhoek                                          ",
        "(UTC+02:00) Athens, Bucharest, Istanbul                       ",
        "(UTC+02:00) Helsinki, Kiev, Riga, Sofia, Tallinn, Vilnius     ",
        "(UTC+02:00) Cairo                                             ",
        "(UTC+02:00) Damascus                                          ",
        "(UTC+02:00) Amman                                             ",
        "(UTC+02:00) Harare, Pretoria                                  ",
        "(UTC+02:00) Jerusalem                                         ",
        "(UTC+02:00) Beirut                                            ",
        "(UTC+03:00) Baghdad                                           ",
        "(UTC+03:00) Minsk                                             ",
        "(UTC+03:00) Kuwait, Riyadh                                    ",
        "(UTC+03:00) Nairobi                                           ",
        "(UTC+03:30) Tehran                                            ",
        "(UTC+04:00) Moscow, St. Petersburg, Volgograd                 ",
        "(UTC+04:00) Tbilisi                                           ",
        "(UTC+04:00) Yerevan                                           ",
        "(UTC+04:00) Abu Dhabi, Muscat                                 ",
        "(UTC+04:00) Baku                                              ",
        "(UTC+04:00) Port Louis                                        ",
        "(UTC+04:30) Kabul                                             ",
        "(UTC+05:00) Tashkent                                          ",
        "(UTC+05:00) Islamabad, Karachi                                ",
        "(UTC+05:30) Sri Jayewardenepura Kotte                         ",
        "(UTC+05:30) Chennai, Kolkata, Mumbai, New Delhi               ",
        "(UTC+05:45) Kathmandu                                         ",
        "(UTC+06:00) Astana                                            ",
        "(UTC+06:00) Dhaka                                             ",
        "(UTC+06:00) Yekaterinburg                                     ",
        "(UTC+06:30) Yangon                                            ",
        "(UTC+07:00) Bangkok, Hanoi, Jakarta                           ",
        "(UTC+07:00) Novosibirsk                                       ",
        "(UTC+08:00) Krasnoyarsk                                       ",
        "(UTC+08:00) Ulaanbaatar                                       ",
        "(UTC+08:00) Beijing, Chongqing, Hong Kong, Urumqi             ",
        "(UTC+08:00) Perth                                             ",
        "(UTC+08:00) Kuala Lumpur, Singapore                           ",
        "(UTC+08:00) Taipei                                            ",
        "(UTC+09:00) Irkutsk                                           ",
        "(UTC+09:00) Seoul                                             ",
        "(UTC+09:00) Osaka, Sapporo, Tokyo                             ",
        "(UTC+09:30) Darwin                                            ",
        "(UTC+09:30) Adelaide                                          ",
        "(UTC+10:00) Hobart                                            ",
        "(UTC+10:00) Yakutsk                                           ",
        "(UTC+10:00) Brisbane                                          ",
        "(UTC+10:00) Guam, Port Moresby                                ",
        "(UTC+10:00) Canberra, Melbourne, Sydney                       ",
        "(UTC+11:00) Vladivostok                                       ",
        "(UTC+11:00) Solomon Islands, New Caledonia                    ",
        "(UTC+12:00) Coordinated Universal Time+12                     ",
        "(UTC+12:00) Fiji, Marshall Islands                            ",
        "(UTC+12:00) Magadan                                           ",
        "(UTC+12:00) Auckland, Wellington                              ",
        "(UTC+13:00) Nuku'alofa                                        ",
        "(UTC+13:00) Samoa                                             ",
    ]

    _unix_time_ss = 1670691600  # unit: second
    _start_milis = round(time.monotonic_ns() / 1000_000)
    _utc_time_hh = 0
    _utc_time_mm = 0
    _wait_dict = {}


def evehp_datetime_millis():
    return  round(time.monotonic_ns() / 1000_000)

def evehp_datetime_waituntil(id, wait_ms):
    if id not in _evehp_datetime_glob._wait_dict:
        _evehp_datetime_glob._wait_dict[id] = evehp_datetime_milis()
        return 0

    now = evehp_datetime_milis()
    dur = now - _evehp_datetime_glob._wait_dict[id]
    if dur > wait_ms:
        _evehp_datetime_glob._wait_dict[id] = now
        return 1
    
    return 0

def evehp_datetime_milis():
    '''return value is the number of milliseconds that have elapsed since the system was started.'''
    return round(time.monotonic_ns() / 1000_000)


def evehp_datetime_set_timezone(unix_time_ss=1670691600, utc_hh_mm='06:00'):
    # unixtimeStamp: number of second from 1970, get i from internet i.e https://www.unixtimestamp.com/
    tm = utc_hh_mm.split(":")
    _evehp_datetime_glob._utc_time_hh = (int)(tm[0])
    _evehp_datetime_glob._utc_time_mm = (int)(tm[1])
    _evehp_datetime_glob._unix_time_ss = unix_time_ss
    _evehp_datetime_glob._start_milis = evehp_datetime_milis()()


def evehp_datetime_now(hh = 0, mm = 0):
    ''' return number of milliseconds in realtime since 1970 Jan 1 '''
    
    unixTS= evehp_datetime_milis() - int(_evehp_datetime_glob._start_milis) + int(_evehp_datetime_glob._unix_time_ss) * 1000
    if hh !=0  or mm !=0:
        second = int(str(unixTS)[0:10])
        tt = time.localtime(second)
        hh_now = tt.tm_hour
        mm_now = tt.tm_min
        
        _evehp_datetime_glob._utc_time_hh = (hh - hh_now)
        _evehp_datetime_glob._utc_time_mm = (mm - mm_now)
    
    return unixTS


def evehp_datetime_now_hh_mm_ss_ms():
    unixTS = evehp_datetime_now()
    second = int(str(unixTS)[0:10])
    tt = time.localtime(second)
    hh = tt.tm_hour + _evehp_datetime_glob._utc_time_hh 
    mm = tt.tm_min + _evehp_datetime_glob._utc_time_mm 
    hh %=24
    mm %=60
    ss = tt.tm_sec
    ms = str(unixTS)[10:13]
    return hh, mm, ss, ms


def evehp_datetime_now_hh_mm_ss_ms_ampm():
    hh, mm, ss, ms = evehp_datetime_now_hh_mm_ss_ms()
    ampm = "AM" if hh < 13 else "PM"
    hh = hh % 12
    return hh, mm, ss, ms, ampm


def evehp_datetime_yyyy_mt_dd_hh_mm_ss_ms_weekday_weeks_weekl_mons_monl():
    unixTS = evehp_datetime_now()
    second = int(str(unixTS)[0:10])
    tt = time.localtime(second)

    ms = str(unixTS)[10:13]
    tt = time.localtime(second)
    yyyy = tt.tm_year - 30
    mt = tt.tm_mon
    dd = tt.tm_mday
    hh = tt.tm_hour + _evehp_datetime_glob._utc_time_hh 
    mm = tt.tm_min + _evehp_datetime_glob._utc_time_mm 
    hh %=24
    mm %=60
    ss = tt.tm_sec
    weekday = tt.tm_wday

    weeks = _print_weekday(weekday, 'short')
    weekl = _print_weekday(weekday, 'long')
    mons = _print_month(mt, 'short')
    monl = _print_month(mt, 'long')
    
    mt = evehp_math_zfill(tt.tm_mon, 2)
    dd = evehp_math_zfill(tt.tm_mday, 2)
    hh = evehp_math_zfill(tt.tm_hour, 2)
    mm = evehp_math_zfill(tt.tm_min, 2)
    ss = evehp_math_zfill(tt.tm_sec, 2)
    weekday = evehp_math_zfill(weekday, 2)
    
    return yyyy, mt, dd, hh, mm, ss, ms, weekday, weeks, weekl, mons, monl


def evehp_datetime_draw_hh_mm_apm(eve: BrtEve, x, y, font=30, color=[255, 255, 255]):
    eve.ColorRGB(color[0], color[1], color[2])
    
    hh, mm, ss, ms, ampm = evehp_datetime_now_hh_mm_ss_ms_ampm()
    eve.cmd_text(x, y, font, eve.OPT_CENTERX, evehp_math_zfill(
        hh, 2) + ":" + evehp_math_zfill(mm, 2) + ":" + evehp_math_zfill(ss, 2) + " " + ampm)


def _print_weekday(weekday, fmt='short'):
    f1 = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]
    f2 = ["Monday", "Tuesday", "Wednessday",
          "Thursday", "Friday", "Saturday", "Sunday"]

    if fmt == 'short':
        return f1[weekday]

    elif fmt == 'long':
        return f2[weekday]

    return f1[weekday]


def _print_month(mon, fmt='short'):
    f1 = ["Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    f2 = ["January", "February", "March", "April", "May", "June", "July", "August",
          "September", "October", "November", "December"]

    mon -= 1

    if fmt == 'short':
        return f1[mon]

    elif fmt == 'long':
        return f2[mon]

    return f1[mon]
