# Импорт/экспорт функциональности динамических библиотек для операционных систем семейства Windows

Фитча добавляет в проект макроопределения переменных WIN_EXPORT_DECLARE и WIN_IMPORT_DECLARE.
В ОС семейства Windows переменные эквивалентны коду

```cpp
#define WIN_EXPORT_DECLARE __declspec(dllexport)
#define WIN_IMPORT_DECLARE __declspec(dllimport)
```

в других ОС
```cpp
#define WIN_EXPORT_DECLARE
#define WIN_IMPORT_DECLARE
```

## История вопроса

При формировании динамических библиотек в операционных системах семейства Windows, в программном коде на C++ указывать атрибуты класса хранения для всех типов и методов, которые предполагается использовать в других программных модулях. В зависимости от ситуации должны быть указан один из двух атрибутов класса хранения - __declspec(dllimport) или __declspec(dllexport). Они указывают компилятору, что функция или объект импортируются или экспортируются (соответственно) из DLL.

Эти атрибуты  определяют интерфейс DLL для клиента, не требуя определения файла модуля (.DEF). Большинство людей находят гораздо проще использовать эти языковые расширения, чем создавать файлы DEF.

Атрибуты __declspec(dllimport) и __declspec(dllexport) взаимосвязаны друг с другом. необходимо использовать dllexport для обозначения экспорта из DLL, и dllimport для обозначения импорта в другой файл. А так как в программном коде обычно используется один и тот же заголовочный файл, как при компиляции DLL, так и при компиляции кода, который потребляет интерфейс DLL, то в декларации должно быть указано __declspec(dllimport) или __declspec(dllexport) в одном и том же месте в зависимости от варианта компиляции.

Для решения данной проблемы обычно используют определения типа

```cpp
#if defined EXPORT_COMPONENT
#	define COMPONENT_WIN_EXPORT WIN_EXPORT_DECLARE
#elif defined IMPORT_COMPONENT
#	define COMPONENT_WIN_EXPORT WIN_IMPORT_DECLARE
#else
#	define COMPONENT_WIN_EXPORT
#endif
```

а затем в программном коде применяют записи подобно

```cpp
class COMPONENT_WIN_EXPORT MyClass
{
    ...
};
```

При этом во время сборки самой DLL задают макроопределение
```pro
DEFINES *= EXPORT_COMPONENT
```

а в других проектах, использующих интерфейс данной DLL задают макроопределение

```pro
DEFINES *= IMPORT_COMPONENT
```

## Универсальное решение с помощью определения единого макроса WIN_EXPORT

В каждой библиотеке можно использовать собственное определение макроса COMPONENT_WIN_EXPORT, однако можно использовать и единое универсальное макроопределение WIN_EXPORT, если в разработке следовать нескольким простым правилам.

* Формировать общий файл WinExport.h в корне исходных файлов модуля

```cpp
#	if defined WIN_EXPORT
#		undef WIN_EXPORT
#	endif

#if defined EXPORT_COMPONENT
#	define WIN_EXPORT WIN_EXPORT_DECLARE
#elif defined IMPORT_COMPONENT
#	define WIN_EXPORT WIN_IMPORT_DECLARE
#else
#	define WIN_EXPORT
#endif
```

* Формировать файлы WinExport.h в подпапках в виде включений этих же файлов верхнего уровня

```cpp
#include "../WinExport.h" // обязательно использовать кавычки ""
```

* Включать файл WinExport.h самым последним включением перед использованием WIN_EXPORT

```cpp
#include <vector>
#include <cmath>
...
#include "WinExport.h" // обязательно включать последним и использовать кавычки ""

class WIN_EXPORT MyClass
{
};
```

### Что это дает?

Применение универсального макроса WIN_EXPORT дает возможность:

* единообразного написания кода;
* переноса функциональности из одного модуля в другой без необходимости изменения самого кода.