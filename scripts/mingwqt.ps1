param (
    [switch]$project,
    [switch]$build,
    [switch]$envpath,
    [switch]$install
)

$download_path = Join-Path $PSScriptRoot "..\tools\qt-mingw.zip"
$extract_path = Join-Path $PSScriptRoot "..\tools\qt-mingw"
$download_url = "https://github.com/liYony/qt-mingw-tool/releases/download/v1.0.0/qt-mingw.zip"

# Check if the installation process should be executed
if ($install) {
    # Check if the compressed file has already been downloaded
    if (Test-Path $download_path) {
        Write-Host "qt-mingw.zip already exists. Skipping download."
    } else {
        # Download the compressed file
        Write-Host "Downloading qt-mingw.zip to $download_path..."
        Invoke-WebRequest -Uri $download_url -OutFile $download_path

        # Check if the download was successful
        if (-not $?) {
            Write-Error "Failed to download qt-mingw.zip"
            exit 1  # Exit with an error code if the download failed
        }
    }

    # Ensure the extraction directory exists, create it if it doesn't
    if (!(Test-Path $extract_path)) {
        New-Item -ItemType Directory -Path $extract_path | Out-Null
    }

    # Extract the file to the $extractDir directory
    Write-Host "Extracting qt-mingw.zip to $extract_path..."
    $extract_parent_path = Join-Path $extract_path ".."
    Expand-Archive -Path $download_path -DestinationPath $extract_parent_path -Force

    # Check if the extraction was successful
    if (-not $?) {
        Write-Error "Failed to extract qt-mingw.zip"
        exit 1  # Exit with an error code if the extraction failed
    }

    # Delete the downloaded zip file (if it was downloaded)
    if (Test-Path $download_path) {
        Write-Host "Deleting qt-mingw.zip..."
        Remove-Item $download_path

        # Check if the file deletion was successful
        if (-not $?) {
            Write-Error "Failed to delete qt-mingw.zip"
            exit 1  # Exit with an error code if the deletion failed
        }
    }

    exit 0  # Exit the script successfully
}

# Save the current directory to the location stack
Push-Location

# Define the path to the MinGW tools directory
$mingw_tool_path = Join-Path $PSScriptRoot "..\tools\qt-mingw"

# Define the path to the MinGW bin directory for the mingw73_64 toolset
$qt_mingw_bin_path = Join-Path $mingw_tool_path "mingw73_64\bin"

# Define the path to the MinGW bin directory for the mingw730_64 toolset
$mingw_bin_path = Join-Path $mingw_tool_path "mingw730_64\bin"

# Define the path to the build directory
$build_path = Join-Path $PSScriptRoot "..\build"

# Define the path to the project file
$project_file = Join-Path $PSScriptRoot "..\project\pinout-generator.pro"

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


if ($env) {
    exit 0
}

# Check if the build directory exists, and create it if it doesn't
if (!(Test-Path -Path $build_path -PathType Container)) {
    Write-Host "Build directory does not exist, creating it..."
    New-Item -Path $build_path -ItemType Directory | Out-Null
}

# Change the current directory to the build directory
Set-Location -Path $build_path

if ($project -or (!$project -and !$build)) {
    # Execute qmake in the build directory
    Write-Host "Executing qmake $project_file in $build_path"

    qmake.exe $project_file

    # Get the exit code of the qmake command
    $lastExitCode = $LASTEXITCODE
    if ($lastExitCode -ne 0) {
        Write-Error "qmake execution failed with exit code $lastExitCode"
    } else {
        Write-Host "qmake execution successful"
    }
}

if ($build -or (!$project -and !$build)) {
    # Execute make in the build directory, using all available cores (-j without a number means use all cores)
    Write-Host "Executing make in $build_path"
    & mingw32-make.exe -j

    # Get the exit code of the make command
    $makeExitCode = $LASTEXITCODE
    if ($makeExitCode -ne 0) {
        Write-Error "make execution failed with exit code $makeExitCode"
    } else {
        Write-Host "make execution successful"
    }
}

# Restore the previous directory from the location stack
Pop-Location
