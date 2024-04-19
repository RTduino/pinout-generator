param (
    [switch]$config,
    [switch]$build,
    [switch]$clean
)

# Save the current directory to the location stack
Push-Location

# Define the path to the build directory
$build_path = Join-Path $PSScriptRoot "..\build"

# Define the path to the project file
$project_file = Join-Path $PSScriptRoot "..\project\pinout-generator.pro"

# Check if the build directory exists, and create it if it doesn't
if (!(Test-Path -Path $build_path -PathType Container)) {
    Write-Host "Build directory does not exist, creating it..."
    New-Item -Path $build_path -ItemType Directory | Out-Null
}

# Change the current directory to the build directory
Set-Location -Path $build_path

if ($clean) {
    # Execute make clean to clean up the build directory
    Write-Host "Executing make clean in $build_path"
    & mingw32-make.exe clean

    # Get the exit code of the make command
    $makeExitCode = $LASTEXITCODE
    if ($makeExitCode -ne 0) {
        Write-Error "make clean execution failed with exit code $makeExitCode"
        Pop-Location
        exit 1
    } else {
        Write-Host "make clean execution successful"
    }
    exit 0
}

if ($config -or (!$config -and !$build)) {
    # Execute qmake in the build directory
    Write-Host "Executing qmake $project_file in $build_path"

    qmake.exe $project_file

    # Get the exit code of the qmake command
    $lastExitCode = $LASTEXITCODE
    if ($lastExitCode -ne 0) {
        Write-Error "qmake execution failed with exit code $lastExitCode"
        Pop-Location
        exit 1
    } else {
        Write-Host "qmake execution successful"
    }
}

if ($build -or (!$config -and !$build)) {
    # Execute make in the build directory, using all available cores (-j without a number means use all cores)
    Write-Host "Executing make in $build_path"
    & mingw32-make.exe -j

    # Get the exit code of the make command
    $makeExitCode = $LASTEXITCODE
    if ($makeExitCode -ne 0) {
        Write-Error "make execution failed with exit code $makeExitCode"
        Pop-Location
        exit 1
    } else {
        Write-Host "make execution successful"
    }
}

# Restore the previous directory from the location stack
Pop-Location
