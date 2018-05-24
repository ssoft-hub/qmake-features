# Набор общих фитч qmake, используемых для сборки программных комплексов

Компонент содержит набор вспомогательных средств, упрощающих ведение проектов с помощью qmake.

## Рекомендации к использованию

Использование фитч возможно в любом проекте.

Подключить фитчи в виде подмодуля в проект в папку project/qmake/features можно с помощью команды:

```bash
git submodule add --name qmake-features git@gitlab.topaz-atcs.com:lemzsdk/qmake-features.git project/qmake/features
```

Можно использовать любой другой путь вместо project/qmake/features, если требуется другое размещение.

Использование фитч в проектах возможно с помощью явного указания их размещения в переменной QMAKEFEATURES

```pro
QMAKEFEATURES = .../project/qmake/features
```

и указания необходимых фитч в переменной CONFIG

```pro
CONFIG *= \
    first_feature \
    second_feature \
```

либо явной загрузкой фитчи с помощью команды load

```pro
load( first_feature ) # загрузка без проверки
!load( second_feature ) : error( Can not find feature \"second_feature\" ) # загрузка с проверкой
```

## Использование в Комплексах

Механизм фитч рекомендуется использовать совместно с разделением проектов на Комплексы и Компоненты

* *Комплекс* - это совокупность компонентов, объединённых вместе в единое законченное целое.
* *Компонент* - это составная часть комплекса.

Компоненты могут иметь зависимости друг от друга, поэтому сами по себе компоненты не используются.
Они всегда являются частью какого-либо комплекса, который должен содержать все необходимые для сборки компоненты.

Рекомендуется хранить все компоненты и комплексы в отдельных репозиториях, и включать Компоненты в общий Комплекс с помощью механизма подмодулей.
В рамках единого комплекса инструмент qmake для всех компонентов позволяет задать общую конфигурацию в файле .qmake.conf.
Содержимое файла .qmake.conf подгружается ко всем проектным файлам уровнями ниже. В проектные файлы такого же уроня и выше .qmake.conf не подгружается.

Таким образом для каждого Комплекса имеется возможность в файле .qmake.conf указать свои собственные особенности сборки, не затрагивая другие Комплексы и сами Компоненты.
Содержимое файла .qmake.conf может выглядеть так

```pro
QMAKEFEATURES = $${PWD}/project/qmake/features
CONFIG *= \
    first_feature \
    second_feature \
```

Проектный файл Комплекса - это обычный проект qmake с подпроектами, например:

```pro
TEMPLATE = subdirs
CONFIG = ordered

SUBDIRS *= \
    first_component \
    second_component \
```

## Использование в Компонентах

Компонент, предполагающий использование данного набора фитч должен быть оформлен определенным образом, и должен содержать

* \<module\>.pro - проектный файл
* \<module\>.prf - файл зависимостей от данного Компонента
 
Файлы могут располагаться в любом месте, независимо друг от друга и даже могут не входить в состав самого Компонента.
То есть, например, для 3rdparty компонентов можно сформировать эти файлы непосредственно в Комплексе.

Проектный файл \<module\>.pro содержит включение файлов исходных кодов и определения, необходимые для сборки Компонента.

```pro
# Тип сборки
TEMPLATE = lib

# Включение исходных файлов
!load( include_files ) : error( Can not find feature \"include_files\" )
includeFiles( $${PWD}/../../src )
```

Файл зависимостей \<module\>.prf содержит определения, необходимые для сборки Компонентов зависимых от этого.

```pro
LIBS *= -l<module>
INCLUDEPATH *= ...
DEPENDPATH *= ...
DEFINES *= ...

QT *= core # зависимости от модулей Qt
DEPENDS = \ # зависимости от других Компонентов
    first_module \
    second_module \
```

## Особенности использования с Qt4

Правила использования фитч и общего файла конфигурации в Qt4 отличается от правил Qt5.

Вместо файла общей конфигурации может быть использован файл .qmake.cache, который должен располагаться в папке сборки.

Для использования фитч, они должны располагаться в подпапке features папки сборки.

Включение в комплекс проекта .qmake.qt4.pro вносит необходимые изменения для возможности сборки инструментами Qt4.
Проект .qmake.qt4.pro в проекте Комплекса должен быть включен первым по счету в переменную SUBDIRS.

## Документация

Каждая фитча сопровождается документацией в виде одноименого <feature>.md файла.