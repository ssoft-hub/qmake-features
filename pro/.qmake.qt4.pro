# Тип проекта
TEMPLATE = subdirs
CONFIG =

# Исправление багов qmake Qt4.
isEqual( QT_MAJOR_VERSION, 4 ) \
{
    QT5_FEATURES_PATH = $$dirname( PWD )

    QMAKE_CONF.pwd = "$${_PRO_FILE_PWD_}"
    QMAKE_CONF.pwd = "$$replace( QMAKE_CONF.pwd, "\\\\", "/" )"
    for( i, 0..1000 ) \
    {
        QMAKE_CONF.file = "$${QMAKE_CONF.pwd}/.qmake.conf"
        exists( $${QMAKE_CONF.file} ) : \
            break()
        unset( QMAKE_CONF.file )
        QMAKE_CONF.pwd = $$dirname( QMAKE_CONF.pwd )
    }

    # Проверяем, что .qmake.conf найден
    !exists( $${QMAKE_CONF.file} ) : \
        error( The file \".qmake.conf\" not found. )

    QT5_BASE_PWD = $${PWD}
    QT4_BASE_PWD = $${OUT_PWD}

    for( index, 0..1000 ) \
    {
        isEqual( QT5_BASE_PWD, $${QMAKE_CONF.pwd} ) : \
            break()
        QT5_BASE_PWD = $$dirname( QT5_BASE_PWD )

        win32 | win64 \
        {
            CHECK_QT4_BASE_PWD = $$lower( $$QT4_BASE_PWD )
            CHECK_CONF_FILE_PWD = $$lower( $$QMAKE_CONF.pwd )
        } \
        else \
        {
            CHECK_QT4_BASE_PWD = $$QT4_BASE_PWD
            CHECK_CONF_FILE_PWD = $$QMAKE_CONF.pwd
        }

        !isEqual( CHECK_QT4_BASE_PWD, $${CHECK_CONF_FILE_PWD} ) : \
            QT4_BASE_PWD = $$dirname( QT4_BASE_PWD )

        unset( CHECK_QT4_BASE_PWD )
        unset( CHECK_CONF_FILE_PWD )
    }

    !isEqual( QT5_BASE_PWD, $${QMAKE_CONF.pwd} ) : \
        error( Can not define paths for Qt4 out. )

    # Копирование фитчей в папку, где qmake Qt4 их найдет
    QT4_FEATURES_PATH = "$${QT4_BASE_PWD}/features"

    win32 | win64 \
    {
        MAKE_FEATURES_COMMAND = mkdir \"$${QT4_FEATURES_PATH}\"
        COPY_FEATURES_COMMAND = xcopy /Y /s \"$${QT5_FEATURES_PATH}\" \"$${QT4_FEATURES_PATH}\"
        COPY_CONFIG_COMMAND = copy /Y \"$${QT5_BASE_PWD}\\.qmake.conf\" \"$${QT4_BASE_PWD}\\.qmake.cache\"
    } \
    else \
    {
        MAKE_FEATURES_COMMAND = mkdir \'$${QT4_FEATURES_PATH}\'
        COPY_FEATURES_COMMAND = cp \'-f\' \'-R\' \'$${QT5_FEATURES_PATH}/\' \'$$dirname(QT4_FEATURES_PATH)/\'
        COPY_CONFIG_COMMAND = cp \'-f\' \'$${QT5_BASE_PWD}/.qmake.conf\' \'$${QT4_BASE_PWD}/.qmake.cache\'
    }

    !exists( $${QT4_FEATURES_PATH} ) \
    {
        message( [$$basename( _PRO_FILE_ )]: Make features path: $${MAKE_FEATURES_COMMAND} )
        !system( $${MAKE_FEATURES_COMMAND} ): \
            error( [$$basename( _PRO_FILE_ )]: Can not make features path )
    } \
    else \
    {
        message( [$$basename( _PRO_FILE_ )]: Find features path: $${QT4_FEATURES_PATH} )
    }

    message( [$$basename( _PRO_FILE_ )]: Copy configuration: $${COPY_CONFIG_COMMAND} )
    !system( $${COPY_CONFIG_COMMAND} ): \
        error( [$$basename( _PRO_FILE_ )]: Can not copy configuration )

    message( [$$basename( _PRO_FILE_ )]: Copy features: $${COPY_FEATURES_COMMAND} )
    !system( $${COPY_FEATURES_COMMAND} ): \
        error( [$$basename( _PRO_FILE_ )]: Can not copy features )
}
