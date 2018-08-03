# Автоматическое включение файлов в проект

## Описание

Фитча предоставляет набор методов для автоматического включения файлов в проект.

* includeFilesFrom - включает файлы из указанной директории.
* includeFiles - включает файлы из указанной директории и всех поддиректорий.

Осуществляется поиск файлов по их расширению а добавление в соответствующие переменные.

```pro
HEADERS *= $$files( $${dir}/*.h )
HEADERS *= $$files( $${dir}/*.hpp )
SOURCES *= $$files( $${dir}/*.c )
SOURCES *= $$files( $${dir}/*.cpp )
FORMS *= $$files( $${dir}/*.ui )
LEXSOURCES *= $$files( $${dir}/*.l )
RES_FILE *= $$files( $${dir}/*.rc )
RESOURCES *= $$files( $${dir}/*.qrc )
TRANSLATIONS *= $$files( $${dir}/*.ts )
DISTFILES *= $$files( $${dir}/*.vert )
DISTFILES *= $$files( $${dir}/*.frag )
```
