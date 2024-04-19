# Define the URL for downloading the compressed file
$download_url = "https://github.com/liYony/qt-mingw-tool/releases/download/v1.0.0/qt-mingw.zip"
# Define the path where the compressed file will be downloaded
$download_path = Join-Path $PSScriptRoot "..\tools\qt-mingw.zip"
# Define the path where the extracted files will be placed
$extract_path = Join-Path $PSScriptRoot "..\tools\qt-mingw"
# Define the path for qmake.exe
$qmake_exe_path = Join-Path $extract_path "mingw73_64\bin\qmake.exe"
# Define the path for gcc.exe
$gcc_exe_path = Join-Path $extract_path "mingw730_64\bin\gcc.exe"

# Check if both qmake.exe and gcc.exe exist
if ((Test-Path $qmake_exe_path) -and (Test-Path $gcc_exe_path)) {
    # If both files exist, skip related operations
    Write-Host "qmake.exe and gcc.exe both exist, skipping related operations."
} else {
    # If any of the files do not exist, perform installation operations
    Write-Host "qmake.exe or gcc.exe does not exist, performing installation operations."

    # Check if the compressed file has already been downloaded
    if (Test-Path $download_path) {
        # If the file already exists, skip the download
        Write-Host "qt-mingw.zip already exists. Skipping download."
    } else {
        # Download the compressed file
        Write-Host "Downloading qt-mingw.zip to $download_path..."
        Invoke-WebRequest -Uri $download_url -OutFile $download_path

        # Check if the download was successful
        if (-not $?) {
            # If the download failed, output an error and exit with an error code
            Write-Error "Failed to download qt-mingw.zip"
            exit 1  # Exit with an error code if the download failed
        }
    }

    # Ensure the extraction directory exists, create it if it doesn't
    if (!(Test-Path $extract_path)) {
        # Create the extraction directory if it doesn't exist
        New-Item -ItemType Directory -Path $extract_path | Out-Null
    }

    # Extract the file to the specified directory
    Write-Host "Extracting qt-mingw.zip to $extract_path..."
    $extract_parent_path = Join-Path $extract_path ".."
    Expand-Archive -Path $download_path -DestinationPath $extract_parent_path -Force

    # Check if the extraction was successful
    if (-not $?) {
        # If the extraction failed, output an error and exit with an error code
        Write-Error "Failed to extract qt-mingw.zip"
        exit 1
    }

    # Delete the downloaded zip file if it exists
    if (Test-Path $download_path) {
        Write-Host "Deleting qt-mingw.zip..."
        Remove-Item $download_path

        # Check if the file deletion was successful
        if (-not $?) {
            # If the deletion failed, output an error and exit with an error code
            Write-Error "Failed to delete qt-mingw.zip"
            exit 1
        }
    }
}
