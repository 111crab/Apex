param(
    [string]$ProjectPath,
    [string]$EngineRoot = "E:\UE_5.8",
    [switch]$KeepAutomationSolution
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($ProjectPath))
{
    $ProjectPath = Join-Path (Split-Path -Parent $PSScriptRoot) "Apex.uproject"
}

$ProjectPath = (Resolve-Path -LiteralPath $ProjectPath).Path
$UnrealBuildTool = Join-Path $EngineRoot "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

if (-not (Test-Path -LiteralPath $UnrealBuildTool))
{
    throw "UnrealBuildTool.exe not found: $UnrealBuildTool"
}

Write-Host "Regenerating Unreal project files..."
Write-Host "Project: $ProjectPath"
Write-Host "UBT: $UnrealBuildTool"

& $UnrealBuildTool -ProjectFiles "-project=$ProjectPath" -game -progress

if ($LASTEXITCODE -ne 0)
{
    exit $LASTEXITCODE
}

$ProjectDir = Split-Path -Parent $ProjectPath
$ProjectName = [System.IO.Path]::GetFileNameWithoutExtension($ProjectPath)
$ExpectedSolution = Join-Path $ProjectDir "$ProjectName.sln"
$ExpectedSolutionX = Join-Path $ProjectDir "$ProjectName.slnx"

if (-not (Test-Path -LiteralPath $ExpectedSolution))
{
    throw "Expected solution was not generated: $ExpectedSolution"
}

if (-not $KeepAutomationSolution)
{
    $AutomationFiles = @(
        (Join-Path $ProjectDir "Automation_$ProjectName.sln"),
        (Join-Path $ProjectDir "Automation_$ProjectName.slnx")
    )

    foreach ($AutomationFile in $AutomationFiles)
    {
        if (Test-Path -LiteralPath $AutomationFile)
        {
            Remove-Item -LiteralPath $AutomationFile -Force
            Write-Host "Removed auxiliary automation solution: $AutomationFile"
        }
    }
}

Write-Host "Open this solution in Rider: $ExpectedSolution"
if (Test-Path -LiteralPath $ExpectedSolutionX)
{
    Write-Host "Generated solutionx: $ExpectedSolutionX"
}

Write-Host "Project files regenerated successfully."
