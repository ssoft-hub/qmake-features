# Задание общих путей сборки компонентов

Применение данной фитчи позволяет задать общие пути для сборки и генерации вспомогательных файлов для всех компонентов комплекса.

## Описание

Пути задаются относительно директории расположения общего файла конфигурации .qmake.conf. Месторасположение общего файла конфигурации .qmake.conf находится с помощью фитчи [qmake_conf_path.prf](qmake_conf_path.md), которая помещает значение директории в переменую QMAKE_CONF_FILE_PWD.

Фитча определяет пути для сборки DESTDIR и для генерируемых файлов в виде

```
DESTDIR = <base>/bin/qt.<version>-<makespec>/<config>/<target>
OBJECTS_DIR = <base>/build/qt.<version>-<makespec>/<config>/<target>/obj
MOC_DIR = <base>/build/qt.<version>-<makespec>/<config>/<target>/moc
RCC_DIR = <base>/build/qt.<version>-<makespec>/<config>/<target>/rcc
UI_DIR = <base>/build/qt.<version>-<makespec>/<config>/<target>/ui
```

* \<base\> - базовая директория (расположения общего файла конфигурации) $${QMAKE_CONF_FILE_PWD}
* \<version\> - версия Qt $${QT_VERSION}
* \<makespec\> - профиль сборки Qt $$basename(QMAKESPEC)
* \<config\> - конфигурация сборки debug|release
* \<target\> - цель сборки (название компонента) $${TARGET}

В значение переменной LIBS значение переменной DESTDIR задается автоматически.

```pro
LIBS *= -L$${DESTDIR}
```

## Зависимости

* [qmake_conf_path.prf](qmake_conf_path.md)
