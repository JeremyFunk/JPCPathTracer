cd "%~dp0

del ..\compile_commands.json

mklink /H ..\compile_commands.json ..\build\compile_commands.json 