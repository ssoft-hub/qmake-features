# Определение атрибутов системы управления версиями git

## Описание

В случае, если в проекте используется система управления версиями git, данная фитча позволяет извлечь атрибуты репозитория, такие как:

* GIT.version - версию программного обеспечения (информацию о самом последнем тэге, доступном из данного коммита) локального репозитория git
```
git describe
```
* GIT.branch - ветка локального репозитория git
```
git symbolic-ref --short HEAD
```
* GIT.revision - SHA1 спецификатор ревизии локального репозитория git
```
git rev-parse --short HEAD
```
* GIT_VERSION.revision_moment - текстовое представление момента фиксации ревизии в формате ISO 8601.
```
git show -s --format=%ci
```
* GIT.pwd - директория извлечения атрибутов локального репозитория git


В случае успешного извлечения атрибутов локального хранилища git, их значения записываются в предопределенные текстовые константы

```cpp
GIT_VERSION
GIT_BRANCH
GIT_REVISION
```

которые могут быть использованы непосредственно в исходных кодах программы

```cpp
#ifndef GIT_VERSION
#define GIT_VERSION "unknown"
#endif
#ifndef GIT_BRANCH
#define GIT_BRANCH "unknown"
#endif
#ifndef GIT_REVISION
#define GIT_REVISION "unknown"
#endif

...

std::cout
    << "Version:" << GIT_VERSION
    << "Revision:" << GIT_REVISION
    << "Branch:" << GIT_BRANCH
    << std::endl;

```

## Особые случаи

По умолчанию в качестве расположения локального репозитория git принимается директория расположения проектного файла .pro.
Если требуется явно задать место расположения локального репозитория git, то значение GIT.pwd необходимо явно задать в проектном файле.

## Сообщения

В случае отсутствия git, либо не заданных тегов или других необходимых атрибутов у хранилища, возможен вывод соответствующих системных или git сообщений.

| Тип        | Сообщение | Описание |
|------------|-----------|----------|
| Информация | Git version of $${TARGET} is $${GIT.version}, SHA1 revision is $${GIT.revision}), branch is $${GIT.branch} | Извлеченная информация о версии, ревизии и ветки локалього репозитория git. |

## Зависимости

* [message_logging.prf](message_logging.md)
* [file_system.prf](file_system.md)
* [include_files.prf](include_files.md)
