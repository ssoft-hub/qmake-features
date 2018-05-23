# Набор общих фитч qmake, используемых для сборки программных комплексов

Компонент содержит набор вспомогательных средств, упрощающих ведение проектов с помощью qmake.

## Рекомендации к использованию

Использование фитч возможно в любом проекте, тем не менее рекомендуется разделять такие понятия, как Комплекс и Компонент.

* *Комплекс* - это совокупность компонентов, объединённых вместе в единое законченное целое.
* *Компонент* - это составная часть комплекса.

Подключить фитчи в виде подмодуля в проект в папку project/qmake/features можно с помощью команды:

<pre>git submodule add --name qmake-features git@gitlab.topaz-atcs.com:lemzsdk/qmake-features.git project/qmake/features</pre>

Используйте другой путь, если требуется другое размещение.

## Документация

Каждая фитча сопровождается документацией в виде одноименого .md файла.