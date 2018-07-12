# Тип проекта
TEMPLATE = subdirs

# Исправление багов qmake Qt4.
isEqual( QT_MAJOR_VERSION, 4 ) {

    QT5_FEATURES_PATH = $$dirname( PWD )

    # Явное включение содержимого фитчи qmake_conf_path
    !include( $${PWD}/../qmake_conf_path.prf ) : error( Can not include feature \"qmake_conf_path\" )

    QT5_BASE_PWD = $${PWD}
    QT4_BASE_PWD = $${OUT_PWD}

    for( index, 1..100 ) {
        isEqual( QT5_BASE_PWD, $${QMAKE_CONF_FILE_PWD} ) : \
            break()
        QT5_BASE_PWD = $$dirname( QT5_BASE_PWD )
        QT4_BASE_PWD = $$dirname( QT4_BASE_PWD )
    }

    !isEqual( QT5_BASE_PWD, $${QMAKE_CONF_FILE_PWD} ) : \
        error( Can not define paths for Qt4 out. )

    # Копирование фитчей в папку, где qmake Qt4 их найдет
    QT4_FEATURES_PATH = $${QT4_BASE_PWD}/features

    win* {
        MAKE_FEATURES_COMMAND = mkdir \"$${QT4_FEATURES_PATH}\"
        COPY_FEATURES_COMMAND = copy /Y \"$${QT5_FEATURES_PATH}\" \"$${QT4_FEATURES_PATH}\"
        COPY_CONFIG_COMMAND = copy /Y \"$${QT5_BASE_PWD}\\.qmake.conf\" \"$${QT4_BASE_PWD}\\.qmake.cache\"
    }

    unix {
        MAKE_FEATURES_COMMAND = mkdir \'$${QT4_FEATURES_PATH}\'
        COPY_FEATURES_COMMAND = find \'$${QT5_FEATURES_PATH}/\' -iname \'*.prf\' -exec cp \'-f\' \'{}\' \'$${QT4_FEATURES_PATH}/\' \';\'
        COPY_CONFIG_COMMAND = cp \'-f\' \'$${QT5_BASE_PWD}/.qmake.conf\' \'$${QT4_BASE_PWD}/.qmake.cache\'
    }

    !exists( $${QT4_FEATURES_PATH} ) {
        message( [$$basename( _PRO_FILE_ )]: Make features path: $${MAKE_FEATURES_COMMAND} )
        !system( $${MAKE_FEATURES_COMMAND} ): \
            error( [$$basename( _PRO_FILE_ )]: Can not make features path )
    } else {
        message( [$$basename( _PRO_FILE_ )]: Find features path: $${QT4_FEATURES_PATH} )
    }

    message( [$$basename( _PRO_FILE_ )]: Copy configuration: $${COPY_CONFIG_COMMAND} )
    !system( $${COPY_CONFIG_COMMAND} ): \
        error( Can not copy configuration )

    message( [$$basename( _PRO_FILE_ )]: Copy features: $${COPY_FEATURES_COMMAND} )
    !system( $${COPY_FEATURES_COMMAND} ): \
        error( Can not copy features )
}
