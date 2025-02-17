using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Android.Content;
using Android.Net;
using Android.Net.Wifi;
using Java.Net;
using Microsoft.Maui.ApplicationModel;
using System.Linq;
using System.Net;
using System.Net.Sockets;

namespace Play_Kazhutha.Platforms.Android
{
    public static class NetworkHelper
    {
        //public static string GetLocalIPAddress()
        //{
        //    var wifiManager = (WifiManager)Platform.CurrentActivity.GetSystemService(Context.WifiService);
        //    var dhcpInfo = wifiManager.DhcpInfo;
        //    int ip = dhcpInfo.IpAddress;
        //    return $"{ip & 0xFF}.{(ip >> 8) & 0xFF}.{(ip >> 16) & 0xFF}.{(ip >> 24) & 0xFF}";
        //}
    }
}
