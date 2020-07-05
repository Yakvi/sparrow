#Build file for sparrow project

$dateFormat = "HH:mm"
function Output-Line([String]$color = "Gray") {
    [String]$m = $matches.Values[0].trim()
    $i = [int] $line.IndexOf($m)
    $line.Substring(0, $i) | Write-Host -ForegroundColor $color -NoNewline
    $line.Substring($i) | Write-Host -ForegroundColor "Gray" 
}

function Output-Logs([String[]]$data, [string]$title = "", [string]$filename = "") {
    $HasErrors = 0
    foreach ($line in $data) {
        If ($line -match "cannot open file '" + $filename) {

        }
        elseif ($line -match "error ") {
            Output-Line("Red")
        }
        elseif ($line -match "warning ") {
            Output-Line("DarkYellow")
        }
        elseif ($line -match "note ") {
            Output-Line("Cyan")
        }
        else {
            Write-Host $line
        }
    }

    if ($HasErrors -eq 1) {
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
$c += '-DSPARROW_WIN32=1'              #Compiles for Win32
# NOTE: Debug mode
$debug = '-DDEBUG=1', '-D_DEBUG=1'     #Basic debug defines
$debug += '-DSPARROW_DEV=1'            #For debug stuff
$debug += '-GR-'                       #Disables run-time type information. For faster compile.
$debug += '-Od'                        #Optimizations disabled
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
$dlllinker += '-PDB:sparrow-' + $(Get-Date -Format mm-ss-ms) + '.pdb'

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
$win32file = "win32\win32_sparrow.c"
# $win32file = "wintest.cpp"
# $debug = "-O2"

$CompileTimer = [System.Diagnostics.Stopwatch]::StartNew()
# NOTE Live code editing: lock running
echo "WAITING FOR PDB" > lock.tmp
del *.pdb 2> lock.tmp

# NOTE WIN32 PLATFORM LAYER
$win32executable = & cl $c $debug $srcDir\$win32file -Fmwin32_sparrow $linker $32linker
Output-Logs -data $win32executable -title "win32 platform layer" -filename "win32_sparrow.exe"

# NOTE sparrow DLL
$sparrow = & cl $c $dllc $debug $srcDir\sparrow.c  $linker $dlllinker
Output-Logs -data $sparrow -title "sparrow dll"

# NOTE Module DLLs
# $writer = & cl $c $dllc $debug $srcDir\mods\writer\mod_writer.c  $linker -EXPORT:ModuleMain -PDB:mod-$(Get-Date -Format mm-ss-ms).pdb
# Output-Logs -data $writer -title "writer dll"

# $everscroll = & cl $c $dllc $debug $srcDir\mods\everscroll\mod_everscroll.c $linker -EXPORT:ModuleMain -PDB:mod-$(Get-Date -Format mm-ss-ms).pdb
# Output-Logs -data $everscroll -title "everscroll dll"

$raycast = & cl $c $dllc $debug -Tp $srcDir\mods\raycast\mod_weekend.cpp  $linker -EXPORT:ModuleMain -PDB:mod-$(Get-Date -Format mm-ss-ms).pdb
Output-Logs -data $raycast -title "raycast dll"


# NOTE Live code editing: resume running
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

