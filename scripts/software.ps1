param (    
    [switch]$repack # A switch parameter to indicate whether to repack the application  
)  
  
# Save the current directory to the location stack  
Push-Location    
  
# Define the paths to the software directory and the Windows software subdirectory  
$software_path = Join-Path $PSScriptRoot "..\software"    
$w_software_path = Join-Path $software_path "pinout-generator-windows"    
  
# Define the path to the executable file to be copied  
$exe_file_path = Join-Path $PSScriptRoot "..\build\release\pinout-generator.exe"  
  
# If the repack switch is specified, clear the target directory and recreate it  
if ($repack) {  
    Write-Host "Clearing the target directory $w_software_path"  
    Remove-Item -Recurse -Force $w_software_path -ErrorAction SilentlyContinue    
    New-Item -ItemType Directory -Path $w_software_path -Force | Out-Null    
}  
  
# Check if the executable file exists  
if (Test-Path $exe_file_path) {    
    Write-Host "The source file $exe_file_path exists, copying to the target directory $w_software_path"    
    Copy-Item $exe_file_path -Destination $w_software_path -Force    
} else {    
    Write-Host "The source file $exe_file_path does not exist, calling mingwqt.ps1 to compile"    
    # Call the mingwqt.ps1 script to compile the executable  
    & .\scripts\mingwqt.ps1    
        
    # Check if the compilation was successful (assuming mingwqt.ps1 returns 0 on success)  
    if ($LASTEXITCODE -eq 0) {    
        Write-Host "Compilation successful, copying pinout-generator.exe to the target directory $w_software_path"    
        Copy-Item $exe_file_path -Destination $w_software_path -Force    
    } else {    
        Write-Host "Compilation failed, unable to copy pinout-generator.exe to the target directory"    
    }    
}  
  
# If the repack switch is specified, run windeployqt to deploy dependencies  
if ($repack) {  
    .\scripts\mingwqt.ps1 -env

    # Change the current directory to the Windows software subdirectory  
    Set-Location -Path $w_software_path 

    & windeployqt.exe pinout-generator.exe  
  
    # Get the exit code of the windeployqt command  
    $lastExitCode = $LASTEXITCODE    
    if ($lastExitCode -ne 0) {    
        Write-Error "windeployqt execution failed with exit code $lastExitCode"    
    } else {    
        Write-Host "windeployqt execution successful"    
    }    
}  
  
# Restore the previous directory from the location stack  
Pop-Location