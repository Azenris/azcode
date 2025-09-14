
#pragma once

Value BuiltInNode_Struct_OS_MkDir( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_Rm( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_Open( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_Close( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_Exists( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_FileSize( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_Read( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_ReadFile( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_OS_Write( Interpreter *interpreter, Value &self, Node *args );