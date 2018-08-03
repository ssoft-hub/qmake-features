# Автоматическая генерация файлов версий компонентов с зависимостями

## Описание

Каждый компонент может иметь версию в какой-либо системе управления версиями (Git, Svn и т.д.).
Если компонет имеет зависимость от других компонентов, то в общем случае их версии
на время сборки этого компонента (build-time) могут отличаться от версий на момент исполнения (run-time).

Важность этого факта состоит в том, что одна и та же версия исполняемого модуля
может иметь различное поведение из-за отличающейся версии динамически подгружаемой библиотеки.

Таким образом, полной версией исполняемого программного модуля является целое версионное дерево для всех составляющих его компонентов.
В случае несоответсвия версии сборки и версии запуска для любого компонента, исполняемый модуль не может считаться корректным.

Рассмотрим следующий пример. Пусть компонент A зависит от B, B от C, а С от D.
Тогда полное дерево зависимостей между компонентами будет иметь вид:

```
A
+- B
|  +- C
|  |  +- D
|  |
|  +- D
|
+- C
|  +- D
|
+- D
```

Если к этому дереву добавть версии, то дерево примет вид

```
A           версия a1a1
+- B        версия b1a1
|  +- C     версия c1b1
|  |  +- D  версия d1c1
|  |
|  +- D     версия d1b1
|
+- C        версия c1a1
|  +- D     версия d1c1
|
+- D        версия d1a1
```

Номера версий b1a1 и т.п. следует читать, как - компонент В версии b1 на момент сборки компонента A версии a1.

Исполняемый модуль будет корректным, если в качестве компонентов будут использованы A(a1), B(b1), C(c1), D(d1).
Если для компонента D в процессе запуска будет использована версию D(d2), то  дерево примет вид:


```
A           версия a1(a1)a1     ok
+- B        версия b1(b1)a1     ok
|  +- C     версия c1(c1)b1     ok
|  |  +- D  версия d1(d2)c1     error
|  |
|  +- D     версия d1(d2)b1     error
|
+- C        версия c1(c1)a1     ok
|  +- D     версия d1(d2)c1     error
|
+- D        версия d1(d2)a1     error
```

В скобках указана версия компонента на момент запуска.

А если для компонента B будет использована верся B(b2), соответсвующая С(с1) и D(d2), то дерево примет вид:

```
A           версия a1(a1)a1     ok
+- B        версия b1(b2)a1     error
|  +- C     версия c1(c1)b2     ok
|  |  +- D  версия d1(d2)c1     error
|  |
|  +- D     версия d2(d2)b2     ok
|
+- C        версия c1(c1)a1     ok
|  +- D     версия d1(d2)c1     error
|
+- D        версия d1(d2)a1     error
```

## Реализация

Представленное дерево реализовано в виде структур C++98 в заголовочном файле [AutoVersion.h](../include/AutoVersion.h).
В каждом проекте путь к директории с этим фалом автоматически добавляется в INCLUDEPATH и DEPENDPATH.
В файле в пространстве имен AutoVersion определены следующие структуры:

* Info - информация о компоненте (product, version, revision и т.д.).
* Dependency - информация о зависимом компоненте на момент сборки зависящего, и версионное дерево зависимого компонета на момент запуска исполняемого модуля.
* Dependencies - список информации о зависимостях Dependency.
* Version - информация о компоненте со списком информации о его зависимостях.

А также перечисляемый признак и методы определения корректности версии.

* Status::Enum - статус версии
    * Invalid - существует разница между значением build-time и run-time в значении ревизии (revision) в дереве версий компонента.
    * Different - существует разница между значением build-time и run-time  в любой информации, за исключением значения ревизии, в дереве версий компонента.
    * Valid -  build-time и run-time информация идентична в дереве версий компонента.
* Status::Enum versionStatus ( const Dependency & value ) - метод проверки статуса зависимости.
* Status::Enum versionStatus ( const Version & value ) - метод проверки статуса версионного дерева.

Фитча определяет следующие атрибуты у переменных COMPLEX и MODULE

* AUTO_VERSION.variant - вариант правила формирования заголовочного и исходного файла.
    * Пустое значение означает генерацию и заголовочного, и исходного файла.
    * header_only значение означает генерацию только заголовочного файла с inline реализацией.
    * app значение означает добавление реализации для метода ::AutoVersion::Version ::AutoVersion::version ();.
* AUTO_VERSION.gen_dir - директория, в которую генерируются файлы учета версий AutoVersion_p.h и AutoVersion_p.cpp для всех проектов.
* AUTO_VERSION.pwd - путь к директории с файлами учета версий AutoVersion_p.h и AutoVersion_p.cpp для данного проекта.
    * Предполагается, что проект не содержит одноименных библиотек (так как иначе будет иметь место неоднозначность определения зависимостей),
    поэтому путь к директории для библиотеки формируется в виде $${AUTO_VERSION.gen_dir}/$${TARGET}.
    * Также предполагается возможность наличия одноименных приложений, поэтому путь к директории для приложений формируется в виде
    "$${AUTO_VERSION.gen_dir}/$${MODULE.base_dir}", MODULE.base_dir - путь к проектному файлу приложения относительно конфигурации комплекса.
* AUTO_VERSION.header_file - путь к заголовочному файлу учета версий $${AUTO_VERSION.pwd}/AutoVersion_p.h.
* AUTO_VERSION.source_file - путь к исходному файлу учета версий $${AUTO_VERSION.pwd}/AutoVersion_p.cpp.
    * В случае варианта AUTO_VERSION.variant равного app, содержит значение AUTO_VERSION.header_file.
* AUTO_VERSION.namespace - пространство имен C++, используемое для реализации методов учета версий.
* AUTO_VERSION.version - текстовое представление версии компонента.
* AUTO_VERSION.revision - текстовое представление о ревизии компонента (номер, SHA1 или т.д.).
* AUTO_VERSION.branch - текстовое представление о ветке компонента.
* AUTO_VERSION.date - текстовое представление даты сборки в формате DD.MM.YYYY.
* AUTO_VERSION.time - текстовое представление о времени сборки в формате hh:mm:ss.
* AUTO_VERSION.product - наименование продукта.
    * Если значение не задано явно, то подставляется значение переменной QMAKE_TARGET_PRODUCT.
    * Если $${QMAKE_TARGET_PRODUCT} не задан, то подставляется значение переменной TARGET.
* AUTO_VERSION.vendor - наименование производителя.
    * Если значение не задано явно, то подставляется значение переменной QMAKE_TARGET_COMPANY.
* AUTO_VERSION.copyright - интелектуальное право.
    * Если значение не задано явно, то подставляется значение переменной QMAKE_TARGET_COPYRIGHT.
* AUTO_VERSION.license - вид лицензирования.
    * Если значение не задано явно, то подставляется значение переменной QMAKE_TARGET_LICENSE.
* AUTO_VERSION.description - краткое описание продукта.
    * Если значение не задано явно, то подставляется значение переменной QMAKE_TARGET_DESCRIPTION.
* AUTO_VERSION.major - цифровое значение мажорной версии продукта.
    * Если значение версии AUTO_VERSION.version имеет вид 1.\*, v.1.\*, v1.\* (\* - любой набор символов), то цифровое значение будет извлечено.
    Иначе AUTO_VERSION.major присваивается значение 1.
* AUTO_VERSION.minor - цифровое значение минорной версии продукта.
    * Если значение версии AUTO_VERSION.version имеет вид 1.2.\*, v.1.2.\*, v1.2.\* (\* - любой набор символов), то цифровое значение будет извлечено.
    Иначе AUTO_VERSION.minor присваивается значение 0.
* AUTO_VERSION.patch - цифровое значение номера патча.
    * Если значение версии AUTO_VERSION.version имеет вид 1.2.3\*, v.1.2.3\*, v1.2.3\* (\* - любой набор символов), то цифровое значение будет извлечено.
    Иначе AUTO_VERSION.patch присваивается значение 0.

Данные атрибуты используются для задания следующих переменных

```pro
CONFIG *= skip_target_version_ext
DEFINES *= QMAKE_AUTO_VERSION_USED
VERSION = $${AUTO_VERSION.major}.$${AUTO_VERSION.minor}.$${AUTO_VERSION.patch}
HEADERS *= "$${AUTO_VERSION.header_file}"
SOURCES *= "$${AUTO_VERSION.source_file}"
```

Чтобы исключить добавление номера мажорной версии к имени исполняемого модуля, задается значение

```pro
CONFIG *= skip_target_version_ext
```

## Процесс генерации

В ходе выпонения фитчи в проекте формируются файлы AutoVersion_p.h и AutoVersion_p.cpp (кроме static библиотек)
следующего содержания

```cpp
// AutoVersion_p.h
#pragma once
#ifndef ComponentName_AutoVersion_H
#define ComponentName_AutoVersion_H

#include <AutoVersion.h>

#ifdef AUTO_VERSION_EXPORT
#undef AUTO_VERSION_EXPORT
#endif

#ifdef EXPORT_ComponentName_AutoVersion
#define AUTO_VERSION_EXPORT __declspec(dllexport)
#else
#define AUTO_VERSION_EXPORT __declspec(dllimport)
#endif

namespace ComponentName_AutoVersion
{
	inline ::AutoVersion::Info info ()
	{
		::AutoVersion::Info result;
		result.product = "ComponentName";
		result.version = "v1.2.3-55e687c";
		result.revision = "55e687c";
		result.date = "02.08.2018";
		result.time = "14:38";
		result.vendor = "Vendor";
		result.copyright = "Copyright";
		result.license = "License";
		result.description = "Description";
		return result;
	}
}

namespace ComponentName_AutoVersion
{
	AUTO_VERSION_EXPORT ::AutoVersion::Version version ();
}

#endif
```

```cpp
// AutoVersion_p.cpp
#include "D:/Working/LemzSDKComplex/build/qt.4.7.4-mingw-gcc.7.1.0-x86/debug/AutoVersion/LemzTools/AutoVersion_p.h"
#include "D:/Working/LemzSDKComplex/build/qt.4.7.4-mingw-gcc.7.1.0-x86/debug/AutoVersion/LemzGlobal/AutoVersion_p.h"

#define EXPORT_ComponentName_AutoVersion
#include "AutoVersion_p.h"

namespace ComponentName_AutoVersion
{
	::AutoVersion::Version version ()
	{
		::AutoVersion::Version result;
		result.info = ::ComponentName_AutoVersion::info();

		// Этот блок повторяется для кадой зависимости
		{
			::AutoVersion::Dependency dependency;
			dependency.info = ::Dependency1_AutoVersion::info();
			dependency.version = ::Dependency1_AutoVersion::version();
			result.dependencies.push_back( dependency );
		}

		return result;
	}
}
```

Для static библиотек содержимое AutoVersion_p.cpp генерируется непосредственно
в AutoVersion_p.h с оформлением виде inline метода.

Для приложений также генерируется реализация для метода ::AutoVersion::Version ::AutoVersion::version (),

```cpp
namespace AutoVersion
{
	::AutoVersion::Version version ()
	{
		return ::ComponentName_Version_AutoVersion::version();
	}
}
```

который может быть использован непосредственно для вывода версии о программе.

```cpp
#include <AutoVersion.h>

void foo ()
{
	AutoVersion::Version version = AutoVersion::version();
	std::cout << "Version: " << version.info.version.c_str << std::endl;
	std::cout << "Revision: " << version.info.revision.c_str << std::endl;
	// ...
}
```

Полное дерево версий может быть выведено, например, с помощью такого кода

```cpp
#ifdef QMAKE_AUTO_VERSION_USED

#include <AutoVersion.h>
#include <iostream>

using namespace ::std;
using namespace ::AutoVersion;

void printHeader ();
void printInfo ( int level, const Info & info );
void printVersion ( int level, const Version & version );
void printDependencies ( int level, const Dependencies & dependencies );

void printHeader ()
{
    cout
        << "----------------------------------------------------------------------------------------------------" << endl
        << "| Status | Product | Version | Revision | Date | Time | Vendor | Copyright | License | Description |" << endl
        << "----------------------------------------------------------------------------------------------------" << endl;
}

void printInfo ( int level, const Info & info )
{
    for ( int i = 0; i < level; ++i )
        cout << ( i == level - 1 ? "+-" : "| " );
    cout << "| "; if ( info.product ) cout << info.product.c_str;
    cout << " | "; if ( info.version ) cout << info.version.c_str;
    cout << " | "; if ( info.revision ) cout << info.revision.c_str;
    cout << " | "; if ( info.date ) cout << info.date.c_str;
    cout << " | "; if ( info.time ) cout << info.time.c_str;
    cout << " | "; if ( info.vendor ) cout << info.vendor.c_str;
    cout << " | "; if ( info.copyright ) cout << info.copyright.c_str;
    cout << " | "; if ( info.license ) cout << info.license.c_str;
    cout << " | "; if ( info.description ) cout << info.description.c_str;
    cout << " |" << endl;
}

void printDependencies ( int level, const Dependencies & dependencies )
{
    Dependencies::const_iterator iter = dependencies.begin();
    while ( iter != dependencies.end() )
    {
        cout << "    ";
        printInfo( level + 1, (*iter).info );       // build-time info
        printVersion( level + 1, (*iter).version ); // run-time version
        ++iter;
    }
}

void printVersion ( int level, const Version & version )
{
    switch ( versionStatus( version ) )
    {
    case Status::Valid: cout << "VAL "; break;
    case Status::Different: cout << "DIF "; break;
    case Status::Invalid: cout << "INV "; break;
    }

    printInfo( level, version.info );                   // version info
    printDependencies( level, version.dependencies );   // version dependencies
}

void printVersion ()
{
    Version version = ::AutoVersion::version();
    printVersion( 0, version );
}

#endif
```

## Сообщения

| Тип        | Сообщение | Описание |
|------------|-----------|----------|
| Ошибка     | Can not find feature ... . | Не найдена фитча ... . |
| Информация | Generate version header file $${AUTO_VERSION.header_file}. | Генерация заголовочного файла $${AUTO_VERSION.header_file}. |
| Информация | Generate version source file $${AUTO_VERSION.source_file}. | Генерация исходного файла $${AUTO_VERSION.source_file}. |

## Зависимости

* [complex_paths.prf](complex_paths.md)
* [module_depends.prf](module_depends.md)
* [git_version.prf](git_version.md)
* [svn_version.prf](svn_version.md)
