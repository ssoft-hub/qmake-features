# Тип проекта
TEMPLATE = subdirs

# Кеширование путей к файлам компонентам (модулям)
DEPENDS_CACHE_CONFIG *= make_depends_cache

load( qmake_conf_path )
OTHER_FILES *= \
    $${QMAKE_CONF.pwd}/.depends.cache
