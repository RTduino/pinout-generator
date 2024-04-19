param (
    [Parameter(Mandatory = $true)]
    [string]$version,
    [Parameter(Mandatory=$false)]
    [ValidateSet("Release", "Alpha", "Beta")]
    [String]$type
)

# Check if the version format is correct
if (!($version -match '^\d+\.\d+\.\d+$')) {
    Write-Error "Invalid version format. Please use format like '2.0.4'."
    exit 1
}

##########################################################################################################################
# version.h

# Define the path to the file to be modified
$filePath = Join-Path $PSScriptRoot "..\project\main\version.h"

# Read the file content
$fileContent = Get-Content -Path $filePath -Raw

# Split the version number into major, minor, and patch parts
$major, $minor, $patch = $version.Split('.')

# Define regular expression patterns to find and replace the version numbers
$majorPattern = '#define PG_VERSION_MAJOR\s+(\d+)\s+.*'
$minorPattern = '#define PG_VERSION_MINOR\s+(\d+)\s+.*'
$patchPattern = '#define PG_VERSION_PATCH\s+(\d+)\s+.*'
$typePattern  = '#define PG_VERSION_TYPE\s+"(Release|Alpha|Beta)"\s+.*'

# Replace the major version number, ensuring proper tab indentation
$fileContent = $fileContent -replace $majorPattern, "#define PG_VERSION_MAJOR                $major               /**< Major version number (X.x.x) */"

# Replace the minor version number, ensuring proper tab indentation
$fileContent = $fileContent -replace $minorPattern, "#define PG_VERSION_MINOR                $minor               /**< Minor version number (x.X.x) */"

# Replace the patch version number, ensuring proper tab indentation
$fileContent = $fileContent -replace $patchPattern, "#define PG_VERSION_PATCH                $patch               /**< Patch version number (x.x.X) */"

if ($type) {
    # Replace the version type, ensuring proper tab indentation
    $fileContent = $fileContent -replace $typePattern, "#define PG_VERSION_TYPE                 `"$type`"       /**< [Alpha] [Beta] [Release] >*/"
}

# Use the .NET File class to save the file
[System.IO.File]::WriteAllText($filePath, $fileContent)

##########################################################################################################################
# updates-gitee.json

# Define the path to the file to be modified
$filePath = Join-Path $PSScriptRoot "..\software\updates-gitee.json"

# Read the file content
$fileContent = Get-Content -Raw -Path $filePath

# Use regular expression to find and replace the version number
$fileContent = $fileContent -replace ('("latest-version":\s*")(\d+(\.\d+)+)(")', ('"latest-version": "' + $version + '"'))

# If needed, update the version number in the download URL (assuming it's the same as latest-version)
$baseUrl = "https://gitee.com/rtduino/pinout-generator/raw/master/software/pinout-generator-v"
$oldDownloadUrlPattern = $baseUrl + '(\d+(\.\d+)+)-setup.exe'
$newDownloadUrl = $baseUrl + $version + "-setup.exe"
$fileContent = $fileContent -replace ($oldDownloadUrlPattern, $newDownloadUrl)

# Use the .NET File class to save the file
[System.IO.File]::WriteAllText($filePath, $fileContent)

##########################################################################################################################
# updates-github.json

# Define the path to the file that will be updated.
$filePath = Join-Path $PSScriptRoot "..\software\updates-github.json"

# Read the contents of the file.
$fileContent = Get-Content -Raw -Path $filePath

# Use regular expressions to find and replace the version number in the file content.
$fileContent = $fileContent -replace ('("latest-version":\s*")(\d+(\.\d+)+)(")', ('"latest-version": "' + $version + '"'))

# If the version number in the download link also needs to be updated, do so here.
$baseUrl = "https://raw.githubusercontent.com/RTduino/pinout-generator/master/software/pinout-generator-v"
$oldDownloadUrlPattern = $baseUrl + '(\d+(\.\d+)+)-setup.exe'
$newDownloadUrl = $baseUrl + $version + "-setup.exe"
$fileContent = $fileContent -replace ($oldDownloadUrlPattern, $newDownloadUrl)

# Use the .NET File class to save the file
[System.IO.File]::WriteAllText($filePath, $fileContent)

##########################################################################################################################
# setup.nsi

# Define the path to the file that needs to be modified
$filePath = Join-Path $PSScriptRoot "..\tools\nsis\setup.nsi"

# Read the entire content of the file as a single string
$fileContent = Get-Content -Raw -Path $filePath

# Use regular expression to find and replace the PRODUCT_VERSION placeholder
# Assuming that the $version variable has been defined and contains the new version number
$fileContent = $fileContent -replace '(!define PRODUCT_VERSION ")\d+(\.\d+)+(")', ('!define PRODUCT_VERSION "' + $version + '"')

# Use the .NET File class to save the file
[System.IO.File]::WriteAllText($filePath, $fileContent)

# Output a success message to the console indicating that the version has been updated successfully.
Write-Host "Version updated successfully."
