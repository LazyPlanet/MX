
@SET OUT_DIR=pb_lua
@SET OPTION= --lua_out=%OUT_DIR% --plugin=protoc-gen-lua="plugin\gen.bat" --proto_path=protocol

xcopy ..\..\*.proto protocol\ 

protoc protocol\*.proto %OPTION%

xcopy .\pb_lua\*.* ..\..\Lua\pb\ /EKRY

@pause