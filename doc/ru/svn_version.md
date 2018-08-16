# Определение атрибутов системы управления версиями svn

## Описание

В случае, если в проекте используется система управления версиями svn, данная фитча позволяет извлечь атрибуты репозитория, такие как:

* SVN.info - полный вывод информации о репозитории svn
* SVN.version - версию программного обеспечения (информацию о самом последнем тэге, доступном из данного коммита) репозитория svn
* SVN.branch - ветка репозитория svn
* SVN.revision - SHA1 спецификатор ревизии репозитория svn
* SVN.pwd - директория извлечения атрибутов репозитория svn

Атрибуты извлекаются с помощью разбора сообщения

```
svn info
```

В случае успешного извлечения атрибутов хранилища svn, их значения записываются в предопределенные текстовые константы

```cpp
SVN_VERSION
SVN_BRANCH
SVN_REVISION
```

которые могут быть использованы непосредственно в исходных кодах программы

```cpp
#ifndef SVN_VERSION
#define SVN_VERSION "unknown"
#endif
#ifndef SVN_BRANCH
#define SVN_BRANCH "unknown"
#endif
#ifndef SVN_REVISION
#define SVN_REVISION "unknown"
#endif

...

std::cout
    << "Version:" << SVN_VERSION
    << "Revision:" << SVN_REVISION
    << "Branch:" << SVN_BRANCH
    << std::endl;

```

## Особые случаи

По умолчанию в качестве расположения репозитория svn принимается директория расположения проектного файла .pro.
Если требуется явно задать место расположения репозитория svn, то значение SVN.pwd необходимо явно задать в проектном файле.

## Сообщения

В случае отсутствия svn, либо не заданных тегов или других необходимых атрибутов у хранилища, возможен вывод соответствующих системных или svn сообщений.

| Тип        | Сообщение | Описание |
|------------|-----------|----------|
| Информация | Svn version of $${TARGET} \($${SVN.pwd}\) is $${SVN.version}, revision is $${SVN.revision}), branch is $${SVN.branch}. | Извлеченная информация о версии, ревизии и ветки локалього репозитория svn. |

## Зависимости

* [message_logging.prf](message_logging.md)
* [file_system.prf](file_system.md)
* [include_files.prf](include_files.md)