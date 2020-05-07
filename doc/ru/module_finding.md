# Поиск файлов зависимостей от компонентов

Вводится соглашение, что файлы с описанием зависимостей от компонентов именуются в виде \<module\>.prf (\<module\> - любое имя компонента).

Результатом применения фитчи является переменная DEPEND_PATH, содержащая перечень директорий с .prf файлами.

Переменная DEPEND_PATH используется фитчей [module_depends.prf](module_depends.md) для поиска и включения содержимого файлов \<module\>.prf при наличии соответствующей зависимости. Зависимость одного компонента от другого указывается посредством переменной DEPENDS.

```pro
DEPENDS *= \<module\>
```

## Кеширование значения DEPEND_PATH

Чтобы не осуществлять поиск файлов с зависимостями для каждого прохода qmake, имеется возможность кешировать значение переменной DEPEND_PATH в файл $${QMAKE_CONF.pwd}/depends.cache. Для осуществления кеширования необходимо для переменной DEPENDS_CACHE_CONFIG добавить значение make_depends_cache

```pro
DEPENDS_CACHE_CONFIG *= make_depends_cache
```

Для принудительного поиска фалов с зависимостями (не использования кешированного значения переменной DEPEND_PATH) необходимо для переменной DEPENDS_CACHE_CONFIG добавить значение unuse_depends_cache

```pro
DEPENDS_CACHE_CONFIG *= unuse_depends_cache
```

## Проектный файл .pro для кеширования значения DEPEND_PATH

Для осуществления однократного поиска путей к файлам \<module\>.prf при сборке комплекса, состоящего из компонентов достаточно добавить дополнительный проект .qmake.cache.pro, в который для переменной DEPENDS_CACHE_CONFIG добавить значение make_depends_cache

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

## Сообщения

| Тип        | Сообщение | Описание |
|------------|-----------|----------|
| Ошибка     | Can not find feature "qmake_conf_path". | Не найдена фитча qmake_conf_path, необходимая для определения расположения общего файла конфигурации ".qmake.conf". |
| Информация | Modules found in: "\<dir\>". | Надены файлы .prf в директории \<dir\>, значение директории добавлено в переменную DEPEND_PATH. |
| Информация | Saving a cached path to "\<base\>/depends.cache". | Значение переменной DEPEND_PATH записано в файл кеша "\<base\>/depends.cache". |
| Информация | Loading a cached path from "\<base\>/depends.cache". | Значение переменной DEPEND_PATH зачитано из файла кеша "\<base\>/depends.cache". |

## Зависимости

* [qmake_conf_path.prf](qmake_conf_path.md)
