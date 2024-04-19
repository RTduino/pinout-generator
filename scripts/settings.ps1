# Define a parameter that can be used to trigger the path input prompt
param (
    [switch]$sysqt,
    [string]$qt_mingw_bin_path,
    [string]$mingw_bin_path
)

# Check if -sysqt switch is used
if ($sysqt) {
    # If -sysqt is used, make sure both paths are provided
    if (!$qt_mingw_bin_path -or !$mingw_bin_path) {
        Write-Error "When using -sysqt, both the Qt MinGW bin path and the MinGW bin path must be provided."
        exit 1
    }
} else {
    & $PSScriptRoot\install.ps1

    # Define the path to the MinGW tools directory
    $mingw_tool_path = Join-Path $PSScriptRoot "..\tools\qt-mingw"
    $qt_mingw_bin_path = Join-Path $mingw_tool_path "mingw73_64\bin"
    $mingw_bin_path = Join-Path $mingw_tool_path "mingw730_64\bin"
}

# Verify that the paths entered by the user exist as directories
if (!(Test-Path -Path $qt_mingw_bin_path -PathType Container)) {
    Write-Error "The path $qt_mingw_bin_path does not exist or is not a directory. Please try again."
    exit 1
}
if (!(Test-Path -Path $mingw_bin_path -PathType Container)) {
    Write-Error "The path $mingw_bin_path does not exist or is not a directory. Please try again."
    exit 1
}

# If the paths are valid, continue with the script
Write-Host "The path for Qt MinGW bin is: $qt_mingw_bin_path"
Write-Host "The path for MinGW bin is: $mingw_bin_path"

# Function to add a path to the PATH environment variable if it doesn't already exist
function add_path_to_envpath($path_to_add) {
    # Get the current PATH environment variable value
    $current_path = $env:PATH -split ';'

    # Check if the path already exists in the PATH environment variable
    if ($current_path -notcontains $path_to_add) {
        # If the path does not exist, add it to the PATH environment variable
        $env:PATH = "$path_to_add;$env:PATH"
        Write-Host "Added path $path_to_add to the PATH environment variable."
    } else {
        Write-Host "Path $path_to_add already exists in the PATH environment variable, skipping addition."
    }
}

# Add the MinGW bin paths to the PATH environment variable if they're not already there
add_path_to_envpath $qt_mingw_bin_path
add_path_to_envpath $mingw_bin_path
