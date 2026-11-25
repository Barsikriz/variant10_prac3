$ErrorActionPreference = "Stop"

$cases = @(
    @{
        Name = "vector_basic"
        Expected = @(
            "2147483648", "1", "2", "4",
            "8", "16", "32", "64"
        )
        Second = @("1", "1", "1", "1", "1", "1", "1", "1")
    },
    @{
        Name = "vector_mixed"
        Expected = @(
            "0", "1", "2147483649", "4026531840",
            "4278190080", "1", "4294901760", "4294965248"
        )
        Second = @("0", "32", "33", "4", "8", "10", "16", "21")
    },
    @{
        Name = "vector_counts"
        Expected = @(
            "9", "2147483652", "1073741826", "536870913",
            "2415919104", "18", "9", "2147483652"
        )
        Second = @("0", "1", "2", "3", "4", "31", "32", "33")
    }
)

$projectRoot = Split-Path -Parent $PSScriptRoot
Push-Location $projectRoot

try {
    New-Item -ItemType Directory -Force .\build | Out-Null

    foreach ($case in $cases) {
        $name = $case.Name
        $binary = ".\build\$name.bin"
        $dumpPath = ".\build\$name.xml"

        & .\assembler.exe --input ".\tests\$name.asm" --output $binary

        if ($LASTEXITCODE -ne 0) {
            throw "Assembly failed: $name"
        }

        if ((Get-Item $binary).Length -ne 224) {
            throw "Unexpected binary size: $name"
        }

        & .\interpreter.exe --input $binary --output $dumpPath --start 100 --end 207

        if ($LASTEXITCODE -ne 0) {
            throw "Execution failed: $name"
        }

        [xml]$dump = Get-Content $dumpPath -Raw
        $cells = @($dump.memory.cell)

        if ($cells.Count -ne 108) {
            throw "Unexpected dump length: $name"
        }

        for ($i = 0; $i -lt $cells.Count; ++$i) {
            if ([string]$cells[$i].address -ne [string](100 + $i)) {
                throw "Unexpected dump address: $name"
            }
        }

        for ($i = 0; $i -lt 8; ++$i) {
            $actual = [string]$cells[$i].value

            if ($actual -ne $case.Expected[$i]) {
                throw "$name element ${i}: expected $($case.Expected[$i]), got $actual"
            }

            $second = [string]$cells[100 + $i].value

            if ($second -ne $case.Second[$i]) {
                throw "$name second vector changed at element $i"
            }
        }

        Write-Host "Passed: $name"
    }

    Write-Host "All vector tests passed."
}
finally {
    Pop-Location
}
