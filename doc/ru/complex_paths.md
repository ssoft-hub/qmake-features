# Определение общих путей сборки компонентов

Использование данной фитчи позволяет задать общие пути для сборки и генерации вспомогательных файлов для всех компонентов комплекса.

## Описание

Пути задаются относительно директории расположения общего файла конфигурации .qmake.conf. Месторасположение общего файла конфигурации .qmake.conf находится с помощью фитчи [qmake_conf_path.prf](qmake_conf_path.md), которая помещает значение директории в переменую QMAKE_CONF_FILE_PWD.

Фитча определяет пути для сборки DESTDIR и для генерируемых файлов в виде

```
DESTDIR = <base>/bin/qt.<version>-<makespec>/<debug|release>/<target>
OBJECTS_DIR = <base>/build/qt.<version>-<makespec>/<debug|release>/<target>/obj
MOC_DIR = <base>/build/qt.<version>-<makespec>/<debug|release>/<target>/moc
RCC_DIR = <base>/build/qt.<version>-<makespec>/<debug|release>/<target>/rcc
UI_DIR = <base>/build/qt.<version>-<makespec>/<debug|release>/<target>/ui
```

* \<base\> - базовая директория (расположения общего файла конфигурации)
* \<version\> - версия Qt
* \<makespec\> - профиль сборки Qt
* \<debug|release\> - режим сборки
* \<target\> - цель сборки (название компонента)

Также автоматически задается значение переменной DESTDIR для переменной LIBS

```pro
LIBS *= -L$${DESTDIR}
```

## Зависимости

* [qmake_conf_path.prf](qmake_conf_path.md)
