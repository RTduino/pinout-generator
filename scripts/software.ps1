param (
    [switch]$repack, # A switch parameter to indicate whether to repack the application
    [String]$exefile
)

# Save the current directory to the location stack
Push-Location

# Define the paths to the software directory and the Windows software subdirectory
$software_path = Join-Path $PSScriptRoot "..\software"
$project_path = Join-Path $PSScriptRoot "..\project"
$third_dll_path = Join-Path $PSScriptRoot "..\tools\third-dll"
$mcus_json_path  = Join-Path $project_path "mcus-json"
$win_software_path = Join-Path $software_path "pinout-generator-windows"
$nsis_script_file = Join-Path $PSScriptRoot "..\tools\nsis\setup.nsi"

# Define the path to the executable file to be copied
$exe_file_path = Join-Path $PSScriptRoot "..\build\release\pinout-generator.exe"

if ($exefile) {  
    if (-not (Test-Path -Path $exefile -PathType Leaf)) {
        Write-Error "The specified executable path $exefile does not exist."  
        exit 1
    }
    $exe_file_path = $exefile
}  

# If the repack switch is specified, clear the target directory and recreate it
if ($repack) {
    Write-Host "Clearing the target directory $win_software_path"
    Remove-Item -Recurse -Force $win_software_path -ErrorAction SilentlyContinue
    New-Item -ItemType Directory -Path $win_software_path -Force | Out-Null
}

# Check if the executable file exists
if (Test-Path $exe_file_path) {
    Write-Host "The source file $exe_file_path exists, copying to the target directory $win_software_path"
    Copy-Item $exe_file_path -Destination $win_software_path -Force
} else {
    Write-Host "The source file $exe_file_path does not exist, calling mingwqt.ps1 to compile"
    # Call the mingwqt.ps1 script to compile the executable
    & $PSScriptRoot\mingwqt.ps1

    # Check if the compilation was successful (assuming mingwqt.ps1 returns 0 on success)
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Compilation successful, copying pinout-generator.exe to the target directory $win_software_path"
        Copy-Item $exe_file_path -Destination $win_software_path -Force
    } else {
        Write-Host "Compilation failed, unable to copy pinout-generator.exe to the target directory"
    }
}

# Get the directories from the PATH environment variable for the machine
$envpath_folders = [Environment]::GetEnvironmentVariable("PATH", "Machine") -split ';' | Where-Object { $_ -ne "" }

# If the repack switch is specified, run windeployqt to deploy dependencies
if ($repack) {
    if ($exefile) {
        # Check each directory to see if it contains windeployqt.exe
        $windeployqt_found = $false
        foreach ($folder in $envpath_folders) {
            $windeployqt_path = Join-Path -Path $folder -ChildPath "windeployqt.exe"
            if (Test-Path -Path $windeployqt_path -PathType Leaf) {
                $windeployqt_found = $true
                Write-Host "windeployqt.exe found at path: $windeployqt_path"
                break # Exit the loop once windeployqt.exe is found
            }
        }

        # If windeployqt.exe is not found
        if (-not $windeployqt_found) {
            Write-Error "windeployqt.exe could not be found. Please ensure it is installed correctly and its path is added to the system's PATH environment variable."
            exit 1 # Exit the script with an error code
        }
    } else {
        & $PSScriptRoot\mingwqt.ps1 -envpath
    }

    # Change the current directory to the Windows software subdirectory
    Set-Location -Path $win_software_path

    & windeployqt.exe pinout-generator.exe

    # Get the exit code of the windeployqt command
    $lastExitCode = $LASTEXITCODE
    if ($lastExitCode -ne 0) {
        Write-Error "windeployqt execution failed with exit code $lastExitCode"
        Pop-Location
        exit 1 # Exit the script with an error code
    } else {
        Write-Host "windeployqt execution successful"
    }

    # mcus-json
    $temp_mcus_path = Join-Path $win_software_path (Split-Path $mcus_json_path -Leaf)

    Copy-Item -Path $mcus_json_path -Destination $temp_mcus_path -Recurse -Force
    Rename-Item -Path $temp_mcus_path -NewName "mcus"

    # third-dll
    Get-ChildItem -Path $third_dll_path -File | Copy-Item -Destination $win_software_path -Force
}

# Check each directory to see if it contains makensis.exe
$makensis_found = $false
foreach ($folder in $envpath_folders) {
    $makensis_path = Join-Path -Path $folder -ChildPath "makensis.exe"
    if (Test-Path -Path $makensis_path -PathType Leaf) {
        $makensis_found = $true
        Write-Host "makensis.exe found at path: $makensis_path"
        break # Exit the loop once makensis.exe is found
    }
}

# If makensis.exe is not found
if (-not $makensis_found) {
    Write-Error "makensis.exe could not be found. Please ensure it is installed correctly and its path is added to the system's PATH environment variable."
    Pop-Location
    exit 1 # Exit the script with an error code
} else {
    # Execute makensis.exe with the provided script file
    makensis.exe $nsis_script_file

    # Get the exit code of the makensis command
    $lastExitCode = $LASTEXITCODE
    if ($lastExitCode -ne 0) {
        Write-Error "makensis execution failed with exit code $lastExitCode"
        Pop-Location
        exit 1 # Exit the script with an error code
    } else {
        Write-Host "makensis execution successful"
    }
}

# Restore the previous directory from the location stack
Pop-Location
