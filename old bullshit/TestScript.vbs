Set objShell = CreateObject("Wscript.shell")
objShell.run("powershell -command ""&{$(Get-PnpDevice -Status ERROR,DEGRADED).FriendlyName}""")