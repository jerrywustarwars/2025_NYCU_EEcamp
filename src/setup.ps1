param (
    [string]$fileName
)

# Bring the window to focus (Replace with the actual window title)
$windowTitle = "game"
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Win32 {
    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);
}
"@
$process = Get-Process | Where-Object { $_.MainWindowTitle -match $windowTitle }
if ($process) {
    [Win32]::SetForegroundWindow($process.MainWindowHandle)
}

# Simulate a click in the center of the screen (if needed)
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public class Mouse {
    [DllImport("user32.dll", SetLastError = true)]
    public static extern void mouse_event(int dwFlags, int dx, int dy, int dwData, int dwExtraInfo);
    public const int MOUSEEVENTF_LEFTDOWN = 0x02;
    public const int MOUSEEVENTF_LEFTUP = 0x04;
    public static void Click() {
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
    }
}
"@

[Mouse]::Click()

# Backup current input language settings
$originalLangs = Get-WinUserLanguageList

# Set input language to English (en-US) temporarily
Set-WinUserLanguageList en-US -Force

# Start the program with the provided filename
$exePath = ".\$fileName"
$process = Start-Process -FilePath $exePath -NoNewWindow -Wait -PassThru
$process.WaitForExit()

# Restore the original input language settings
Set-WinUserLanguageList $originalLangs -Force
Set-WinUserLanguageList en-US -Force
Set-WinUserLanguageList zh-TW -Force