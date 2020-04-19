#Build file for sparrow project

$dateFormat = "HH:mm"
function Output-Logs([String[]]$data, [string]$title = "") {
    foreach ($line in $data) {
        If ($line -match "error ") {
            [string]$m = $matches.Values[0].trim()
            $i = $line.IndexOf($m)
            $line.Substring(0, $i) | Write-Host -ForegroundColor "Red" -NoNewline
            $line.Substring($i) | Write-Host -ForegroundColor "Gray" 
        }
        elseif ($line -match "warning ") {
            [string]$m = $matches.Values[0].trim()
            $i = $line.IndexOf($m)
            $line.Substring(0, $i) | Write-Host -ForegroundColor "DarkYellow" -NoNewline
            $line.Substring($i) | Write-Host -ForegroundColor "Gray" 
        }
        elseif ($line -match "note ") {
            [string]$m = $matches.Values[0].trim()
            $i = $line.IndexOf($m)
            $line.Substring(0, $i) | Write-Host -ForegroundColor "Cyan" -NoNewline
            $line.Substring($i) | Write-Host -ForegroundColor "Gray" 
        }
        else {
            Write-Host $line
        }
    }

    if ($data -match "error") {
        Write-Host "[$(Get-Date -Format $dateFormat)]: " -ForegroundColor "Yellow" -NoNewLine 
        Write-Host "Compilation failed, " -ForegroundColor "Red" -NoNewLine
        Write-Host $title -ForegroundColor "Cyan"
    }
}

### BOOKMARK: End helper function

$project = "sparrow"
$buildDir = "build"

# NOTE: Compiler flags
$c = '-nologo','-FC'                   #Display full path of source code
# NOTE: Faster compile/runtime
$c += '-fp:fast'                       #Floating point behaviour. Precise behaviour usually unnecessary.
$c += '-fp:except-'                    #Floating point behaviour. Precise behaviour usually unnecessary.
$c += '-Gm-'                           #Enables minimal rebuild. For faster compile.
$c += '-Oi'                            #Generates intrinsic functions. For faster runtime.
$c += '-EHa-'                          #Disable exception handling, -EHsc for c++
$c += '-GS-'                           #Disable Buffer Security checks https://docs.microsoft.com/en-us/cpp/build/reference/gs-buffer-security-check
$c += '-Gs9999999'                     #Push back __chkstk threshold https://hero.handmade.network/forums/code-discussion/t/94-guide_-_how_to_avoid_c_c++_runtime_on_windows
# NOTE: Preprocessor directives
$c += '-DSPARROW_DEV=1'                #For debug stuff
$c += '-DSPARROW_WIN32=1'              #Compiles for Win32
# NOTE: Debug mode
$debug = '-DDEBUG=1', '-D_DEBUG=1'     #Basic debug defines
$debug += '-GR-'                       #Disables run-time type information. For faster compile.
# $debug += '-Od'                        #Optimizations disabled
$debug += '-MTd'                       #Creates debug multithreaded executable
$debug += '-Zo'                        #Enhance Optimized Debugging
$debug += '-Z7'                        #Generates C 7.0–compatible debugging information.
$debug += '-WX'                        #Treats all warnings as errors (except the ones below).
$debug += '-W3' #'-Wall'                        #Displays all warnings (except the ones below).
# NOTE: Ignoring selected warnings:
$debug += '-wd4100'                  #Unreferenced variable
# $debug += '-wd4711'                  #Function inlined by optimizer
# $debug += '-wd4189'                  #Local variable initialized but not used
# $debug += '-wd4505'                  #Unreferenced local function
# $debug += '-wd4201'                  #Nonstandard extension used: nameless struct/union
# $debug += '-wd4127'                  #
# $debug += '-wd4302'                  #pointer truncation
# $debug += '-wd4311'                  #truncation
# $debug += '-wd4838'                  #Conversion from type 1 to type 2 requires a narrowing conversion
# $debug += '-wd4456'                  #Declaration hides previous local declaration

# NOTE: linker flags, go after the source file
$linker = '/link', '-incremental:no'   #Passes linker parameters from here; Disables incremental linking of the linker
$linker += '-opt:ref'                  #Eliminates functions and data that are never referenced
$linker += '-NODEFAULTLIB'             #Disable standard C library
$linker += '-STACK:0x100000,0x100000'  #Access to full stack, https://hero.handmade.network/forums/code-discussion/t/94-guide_-_how_to_avoid_c_c++_runtime_on_windows

# NOTE: Extra libraries for win32
$32linker = 'kernel32.lib', 'user32.lib'
$32linker += 'gdi32.lib'
# $32linker += 'shlwapi.lib'             # Shell API
# $32linker += 'msvcrt.lib'           #REMINDER: This program is compiled in x64!
# $32linker += '-LIBPATH:H:\C\_Deps\Lib'
# $32linker += 'winmm.lib'
# $32linker += 'shell32.lib'
# NOTE: Extra parameters for sparrow.dll 
$dllc = '-FmSPARROW', '-LD'
$dllc += '-GR-'              
$dlllinker += '-EXPORT:UpdateState', '-EXPORT:Render'

#timeout /t 1
$srcDir = "..\src"

# Remove-Item -Path ..\$buildDir -Force -Recurse # NOTE: Clean build
if(!(Test-Path -Path ..\$buildDir)) { mkdir ..\$buildDir }
pushd ..\$buildDir
Clear
Write-Host "[$(Get-Date -Format $dateFormat)]: " -ForegroundColor "Yellow" -NoNewLine 
Write-Host "Compilation started." -ForegroundColor "Cyan"
Write-Host ""

### BOOKMARK: Actual compiler calls
$win32file = "win32_sparrow.c"
$opt = "-Od"
# $win32file = "wintest.cpp"

$CompileTimer = [System.Diagnostics.Stopwatch]::StartNew()
# NOTE WIN32 PLATFORM LAYER
$win32executable = & cl $c $debug $opt $srcDir\$win32file -Fmwin32_sparrow $linker $32linker
Output-Logs -data $win32executable -title "win32 platform layer"

echo "WAITING FOR PDB" > lock.tmp
# NOTE sparrow DLL
$sparrow = & cl $c $dllc $debug $opt $srcDir\sparrow.c  $linker $dlllinker
Output-Logs -data $sparrow -title "sparrow dll"
del lock.tmp

# TODO: This would be cool to try porting to our engine in pure C
# $win32executable = & cl $c -EHsc $srcDir\experiments\confps.cpp -Fmwin32_sparrow $linker $32linker
# Output-Logs -data $win32executable -title "Test: FPS in console (C++)"

popd

# NOTE: Compiling Diagnostics
$CompileTime = $CompileTimer.Elapsed
Write-Host 
Write-Host "[$(Get-Date -Format $dateFormat)]: " -ForegroundColor "Yellow" -NoNewLine 
Write-Host "Compilation finished in " -ForegroundColor "Cyan"              -NoNewLine
Write-Host $([string]::Format("{0:d1}s {1:d3}ms", $CompileTime.seconds, $CompileTime.milliseconds)) -ForegroundColor "Green"

