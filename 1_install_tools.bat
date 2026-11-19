@echo off
echo ============================================================
echo  VocalForge: installing build tools (CMake, Git, VS Build Tools)
echo  This downloads several GB and takes 10-30 minutes.
echo ============================================================
winget install -e --id Kitware.CMake --accept-package-agreements --accept-source-agreements
winget install -e --id Git.Git --accept-package-agreements --accept-source-agreements
winget install -e --id Microsoft.VisualStudio.2022.BuildTools --accept-package-agreements --accept-source-agreements --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
echo.
echo Done. Now CLOSE this window, then right-click 2_build_and_install.bat ^> Run as administrator.
pause
