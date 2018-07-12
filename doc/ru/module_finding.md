# Поиск файлов зависимостей от компонентов

Вводится соглашение, что файлы с описанием зависимостей от компонентов именуются в виде ModuleName.prf (ModuleName - любое имя компонента).

Загрузка фитчи формирует переменную DEPEND_PATH, содержащую перечень директорий с .prf файлами. Переменная DEPEND_PATH используется фитчей module_depends для поиска и включения содержимого файлов ModuleName.prf при наличии соответствующей зависимости. Зависимость одного компонента от другого указывается посредством переменной DEPENDS.

```pro
DEPENDS *= ModuleName
```

## Кеширование значения DEPEND_PATH

Чтобы не осуществлять поиск файлов с зависимостями для каждого прохода qmake, имеется возможность кешировать значение переменной DEPEND_PATH в файл $${QMAKE_CONF_FILE_PWD}/.depends.cache. Для осуществления кеширования необходимо для переменной DEPENDS_CACHE_CONFIG добавить значение make_depends_cache

```pro
DEPENDS_CACHE_CONFIG *= make_depends_cache
```

Для принудительного поиска фалов с зависимостями (не использования кешированного значения переменной DEPEND_PATH) необходимо для переменной DEPENDS_CACHE_CONFIG добавить значение unuse_depends_cache

```pro
DEPENDS_CACHE_CONFIG *= unuse_depends_cache
```

## Проектный файл .pro для кеширования значения DEPEND_PATH

Для осуществления однократного поиска путей к файлам ModuleName.prf при сборке комплекса, состоящего из компонентов достаточно доабвить дополнительный проект .qmake.cache.pro, в который для переменной DEPENDS_CACHE_CONFIG добавить значение make_depends_cache

```pro
TEMPLATE = subdirs
DEPENDS_CACHE_CONFIG *= make_depends_cache
```

 В общем проекте комплекса проектный файл .qmake.cache.pro должен быть указан до проектных файлов компонентов, но позднее .qmake.qt4.pro (если он используется).
 
 ```pro
TEMPLATE = subdirs

CONFIG *= ordered

SUBDIRS *= \
    $${PWD}/.qmake.qt4.pro \
    $${PWD}/.qmake.cache.pro \
    # ...
```

## Зависимости

* [qmake_conf_path.prf](qmake_conf_path.md)