# Variables for font settings - change these as needed
$FontName = "Consolas"
$FontSize = 18

# Find the settings.json file location
$SettingsPath = "$env:LOCALAPPDATA\Packages\Microsoft.WindowsTerminal_8wekyb3d8bbwe\LocalState\settings.json"

# Check alternate locations if the first one doesn't exist
if (-not (Test-Path $SettingsPath)) {
    $SettingsPath = "$env:LOCALAPPDATA\Microsoft\Windows Terminal\settings.json"
}

# Verify file exists
if (-not (Test-Path $SettingsPath)) {
    Write-Host "Windows Terminal settings file not found." -ForegroundColor Red
    Write-Host "Looked in:"
    Write-Host "$env:LOCALAPPDATA\Packages\Microsoft.WindowsTerminal_8wekyb3d8bbwe\LocalState\settings.json"
    Write-Host "$env:LOCALAPPDATA\Microsoft\Windows Terminal\settings.json"
}
else {
    # Create backup
    Write-Host "Creating backup of settings file..."
    Copy-Item $SettingsPath "$SettingsPath.backup"

    # Read and modify the JSON file
    try {
        Write-Host "Updating font settings to $FontName size $FontSize..."
        $settings = Get-Content $SettingsPath -Raw | ConvertFrom-Json

        # Ensure defaults section exists
        if (-not $settings.profiles.PSObject.Properties.Name -contains 'defaults') {
            $settings.profiles | Add-Member -Name 'defaults' -Value ([PSCustomObject]@{}) -MemberType NoteProperty
        }

        # Update font settings
        $settings.profiles.defaults | Add-Member -Name 'fontFace' -Value $FontName -MemberType NoteProperty -Force
        $settings.profiles.defaults | Add-Member -Name 'fontSize' -Value $FontSize -MemberType NoteProperty -Force

        # Save the updated settings
        $settingsJson = ConvertTo-Json -InputObject $settings -Depth 100
        [System.IO.File]::WriteAllText($SettingsPath, $settingsJson)

        Write-Host "Windows Terminal font settings updated successfully." -ForegroundColor Green
        Write-Host "Please restart Windows Terminal to see the changes."
    }
    catch {
        Write-Host "Error occurred: $_" -ForegroundColor Red
    }
}
