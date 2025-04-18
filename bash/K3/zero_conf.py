#!/usr/bin/env python3

from zeroconf import ServiceBrowser, ServiceListener, Zeroconf, IPVersion
import time

ListIP = []

class MyListener(ServiceListener):

    def add_service(self, zc: Zeroconf, type_: str, name: str) -> None:
        info = zc.get_service_info(type_, name)
        # print(f"{info.parsed_addresses()[0]}")
        # print(info.get_name(), info.parsed_addresses(IPVersion.V4Only))
        ListIP.append(info.parsed_addresses(IPVersion.V4Only)[0])


zeroconf = Zeroconf()
listener = MyListener()
browser = ServiceBrowser(zeroconf, "_http._tcp.local.", listener)

try:
    time.sleep(5)
finally:
    zeroconf.close()

print(ListIP)
