[CmdletBinding()]
param (
    [string] $archiveName, [string] $targetName
)
# 外部环境变量包括:
# archiveName: ${{ matrix.qt_ver }}-${{ matrix.qt_arch }}
# winSdkDir: ${{ steps.build.outputs.winSdkDir }}
# winSdkVer: ${{ steps.build.outputs.winSdkVer }}
# vcToolsInstallDir: ${{ steps.build.outputs.vcToolsInstallDir }}
# vcToolsRedistDir: ${{ steps.build.outputs.vcToolsRedistDir }}
# msvcArch: ${{ matrix.msvc_arch }}


# winSdkDir: C:\Program Files (x86)\Windows Kits\10\ 
# winSdkVer: 10.0.19041.0\ 
# vcToolsInstallDir: C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\14.28.29333\ 
# vcToolsRedistDir: C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Redist\MSVC\14.28.29325\ 
# archiveName: 5.9.9-win32_msvc2015
# msvcArch: x86

$scriptDir = $PSScriptRoot
$currentDir = Get-Location
Write-Host "currentDir" $currentDir
Write-Host "scriptDir" $scriptDir

function Main() {
    New-Item -ItemType Directory $archiveName
    New-Item -ItemType Directory $archiveName\locale
    # download english tessdata
    Invoke-WebRequest -Uri "https://github.com/GSam/Capture2Text/releases/download/Prototype/English.zip" -OutFile $archiveName'\English.zip'
    #extract zip
    Expand-Archive -LiteralPath $archiveName\English.zip -DestinationPath $archiveName\tessdata
    # 拷贝exe
    Copy-Item release\$targetName $archiveName\
    Write-Host "copy item finished..."
    # 拷贝依赖
    windeployqt --compiler-runtime $archiveName\$targetName
    # 删除不必要的文件
    $excludeList = @("*.qmlc", "*.ilk", "*.exp", "*.lib", "*.pdb")
    Remove-Item -Path $archiveName -Include $excludeList -Recurse -Force
    Write-Host "remove item finished..."
    # 拷贝vcRedist dll
    # $redistDll="{0}{1}\*.CRT\*.dll" -f $env:vcToolsRedistDir.Trim(),$env:msvcArch
    # Write-Host "redist dll $($redistDll)"
    # Copy-Item $redistDll $archiveName\
    # Write-Host "copy redist dll..."
    Copy-Item "LICENSE" $archiveName\
    Write-Host "copy license.."
    # 拷贝WinSDK dll
    # $sdkDll="{0}Redist\{1}ucrt\DLLs\{2}\*.dll" -f $env:winSdkDir.Trim(),$env:winSdkVer.Trim(),$env:msvcArch
    # Write-Host "copy sdk dll$($sdkDll)"
    # Copy-Item $sdkDll $archiveName\
    Copy-Item lib\*.dll $archiveName\
    # Copy-Item locale\*.qm $archiveName\locale\

    Write-Host "compress zip..."
    # 打包zip
    Compress-Archive -Path $archiveName -DestinationPath $archiveName'.zip'
}

if ($null -eq $archiveName || $null -eq $targetName) {
    Write-Host "args missing, archiveName is" $archiveName ", targetName is" $targetName
    return
}
Main


