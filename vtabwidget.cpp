#include <QtWidgets>
#include <QtDebug>
#include "vtabwidget.h"
#include "vedit.h"

VTabWidget::VTabWidget(const QString &welcomePageUrl, QWidget *parent)
    : QTabWidget(parent), welcomePageUrl(welcomePageUrl)
{
    setTabsClosable(true);
    connect(tabBar(), &QTabBar::tabCloseRequested,
            this, &VTabWidget::handleTabCloseRequest);

    openWelcomePage();
}

void VTabWidget::openWelcomePage()
{
    int idx = openFileInTab(welcomePageUrl, "");
    setTabText(idx, "Welcome to VNote");
    setTabToolTip(idx, "VNote");
}

int VTabWidget::insertTabWithData(int index, QWidget *page, const QString &label,
                                  const QJsonObject &tabData)
{
    int idx = insertTab(index, page, label);
    QTabBar *tabs = tabBar();
    tabs->setTabData(idx, tabData);
    Q_ASSERT(tabs->tabText(idx) == label);
    return idx;
}

int VTabWidget::appendTabWithData(QWidget *page, const QString &label, const QJsonObject &tabData)
{
    return insertTabWithData(count(), page, label, tabData);
}

void VTabWidget::openFile(QJsonObject fileJson)
{
    if (fileJson.isEmpty()) {
        return;
    }
    qDebug() << "open file:" << fileJson;

    QString path = fileJson["path"].toString();
    QString name = fileJson["name"].toString();

    // Find if it has been opened already
    int idx = findTabByFile(path, name);
    if (idx > -1) {
        setCurrentIndex(idx);
        return;
    }

    idx = openFileInTab(path, name);

    setCurrentIndex(idx);
}

int VTabWidget::openFileInTab(const QString &path, const QString &name)
{
    VEdit *edit = new VEdit(path, name);
    QJsonObject tabJson;
    tabJson["path"] = path;
    tabJson["name"] = name;
    int idx = appendTabWithData(edit, name, tabJson);
    setTabToolTip(idx, path);
    return idx;
}

int VTabWidget::findTabByFile(const QString &path, const QString &name)
{
    QTabBar *tabs = tabBar();
    int nrTabs = tabs->count();

    for (int i = 0; i < nrTabs; ++i) {
        QJsonObject tabJson = tabs->tabData(i).toJsonObject();
        if (tabJson["name"] == name && tabJson["path"] == path) {
            return i;
        }
    }
    return -1;
}

void VTabWidget::handleTabCloseRequest(int index)
{
    qDebug() << "request closing tab" << index;
    VEdit *edit = dynamic_cast<VEdit *>(widget(index));
    bool ok = edit->requestClose();
    if (ok) {
        removeTab(index);
        delete edit;
    }
}