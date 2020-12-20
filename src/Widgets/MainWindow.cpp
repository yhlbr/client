#include "MainWindow.h"
#include "AboutDialog.h"
#include "HelpDialog.h"
#include "SettingsDialog.h"

#include "Version.h"
#include "Global.h"

#include "EventManager.h"
#include "DatabaseManager.h"
#include "Events/ExportPresetEvent.h"
#include "Events/ImportPresetEvent.h"
#include "Events/SaveAsPresetEvent.h"
#include "Events/ToggleFullscreenEvent.h"
#include "Events/Rundown/CloseRundownEvent.h"
#include "Events/Rundown/EmptyRundownEvent.h"
#include "Events/Rundown/NewRundownEvent.h"
#include "Events/Rundown/OpenRundownEvent.h"
#include "Events/Rundown/SaveRundownEvent.h"
#include "Events/Rundown/CopyItemPropertiesEvent.h"
#include "Events/Rundown/PasteItemPropertiesEvent.h"
#include "Events/Library/RefreshLibraryEvent.h"
#include "Events/Rundown/AllowRemoteTriggeringMenuEvent.h"
#include "Events/Rundown/CompactViewEvent.h"
#include "Events/Rundown/ExecutePlayoutCommandEvent.h"
#include "Events/Rundown/AllowRemoteTriggeringEvent.h"

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setupUi(this);
    setupMenu();
    setWindowIcon(QIcon(":/Graphics/Images/CasparCG.png"));

#if defined(Q_OS_MAC)
    verticalLayoutLeft->setSpacing(5);
#endif

    this->applicationTitle = this->windowTitle();

    this->widgetAction->setVisible(false);

    this->splitterHorizontal->setSizes(QList<int>() << 1 << 0);
    this->splitterVertical->setSizes(QList<int>() << 289 << 860 << 289);

    bool showPreviewPanel = (DatabaseManager::getInstance().getConfigurationByName("ShowPreviewPanel").getValue() == "true") ? true : false;
    this->widgetPreview->setVisible(showPreviewPanel);

    bool showLivePanel = (DatabaseManager::getInstance().getConfigurationByName("ShowLivePanel").getValue() == "true") ? true : false;
    this->widgetLive->setVisible(showLivePanel);

    bool showAudioLevelsPanel = (DatabaseManager::getInstance().getConfigurationByName("ShowAudioLevelsPanel").getValue() == "true") ? true : false;
    this->widgetAudioLevels->setVisible(showAudioLevelsPanel);

    bool showDurationPanel = (DatabaseManager::getInstance().getConfigurationByName("ShowDurationPanel").getValue() == "true") ? true : false;
    this->widgetDuration->setVisible(showDurationPanel);

    QObject::connect(&EventManager::getInstance(), SIGNAL(statusbar(const StatusbarEvent&)), this, SLOT(statusbar(const StatusbarEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(emptyRundown(const EmptyRundownEvent&)), this, SLOT(emptyRundown(const EmptyRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(activeRundownChanged(const ActiveRundownChangedEvent&)), this, SLOT(activeRundownChanged(const ActiveRundownChangedEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(newRundownMenu(const NewRundownMenuEvent&)), this, SLOT(newRundownMenu(const NewRundownMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(openRundownMenu(const OpenRundownMenuEvent&)), this, SLOT(openRundownMenu(const OpenRundownMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(openRundownFromUrlMenu(const OpenRundownFromUrlMenuEvent&)), this, SLOT(openRundownFromUrlMenu(const OpenRundownFromUrlMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(compactView(const CompactViewEvent&)), this, SLOT(compactView(const CompactViewEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(allowRemoteTriggering(const AllowRemoteTriggeringEvent&)), this, SLOT(allowRemoteTriggering(const AllowRemoteTriggeringEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(repositoryRundown(const RepositoryRundownEvent&)), this, SLOT(repositoryRundown(const RepositoryRundownEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(exportPresetMenu(const ExportPresetMenuEvent&)), this, SLOT(exportPresetMenu(const ExportPresetMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(saveAsPresetMenu(const SaveAsPresetMenuEvent&)), this, SLOT(saveAsPresetMenu(const SaveAsPresetMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(reloadRundownMenu(const ReloadRundownMenuEvent&)), this, SLOT(reloadRundownMenu(const ReloadRundownMenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(switchRundown1Menu(const SwitchRundown1MenuEvent&)), this, SLOT(switchRundown1Menu(const SwitchRundown1MenuEvent&)));
    QObject::connect(&EventManager::getInstance(), SIGNAL(switchRundown2Menu(const SwitchRundown2MenuEvent&)), this, SLOT(switchRundown2Menu(const SwitchRundown2MenuEvent&)));
}

void MainWindow::setupMenu()
{
    this->openRecentMenu = new QMenu(this);
    this->openRecentMenu->setTitle("Open Recent Rundown");
    QObject::connect(this->openRecentMenu, SIGNAL(triggered(QAction*)), this, SLOT(openRecentMenuActionTriggered(QAction*)));

    this->fileMenu = new QMenu(this);
    this->newRundownAction = this->fileMenu->addAction("New Rundown", this, SLOT(newRundown()), QKeySequence::fromString("Ctrl+N"));
    this->openRundownAction = this->fileMenu->addAction("Open Rundown...", this, SLOT(openRundown()), QKeySequence::fromString("Ctrl+O"));
    this->openRundownFromUrlAction = this->fileMenu->addAction("Open Rundown from repository...", this, SLOT(openRundownFromUrl()), QKeySequence::fromString("Ctrl+Shift+O"));
    this->fileMenu->addSeparator();
    this->openRecentMenuAction = this->fileMenu->addMenu(this->openRecentMenu);
    this->fileMenu->addSeparator();
    this->fileMenu->addAction("Import Preset...", this, SLOT(importPreset()));
    this->exportPresetAction = this->fileMenu->addAction("Export Preset...", this, SLOT(exportPreset()));
    this->saveAsPresetAction = this->fileMenu->addAction("Save as Preset...", this, SLOT(saveAsPreset()));
    this->fileMenu->addSeparator();
    this->saveAction = this->fileMenu->addAction("Save", this, SLOT(saveRundown()), QKeySequence::fromString("Ctrl+S"));
    this->saveAsAction = this->fileMenu->addAction("Save As...", this, SLOT(saveAsRundown()), QKeySequence::fromString("Ctrl+Shift+S"));
    this->fileMenu->addSeparator();
    this->fileMenu->addAction("Quit", this, SLOT(close()));
    this->saveAsPresetAction->setEnabled(false);
    QObject::connect(this->openRecentMenuAction, SIGNAL(hovered()), this, SLOT(openRecentMenuHovered()));

    this->editMenu = new QMenu(this);
    this->editMenu->addAction("Settings...", this, SLOT(showSettingsDialog()));

    this->viewMenu = new QMenu(this);
    //this->viewMenu->addSeparator();
    this->viewMenu->addAction("Toggle Fullscreen", this, SLOT(toggleFullscreen()), QKeySequence::fromString("Ctrl+F"));

    this->libraryMenu = new QMenu(this);
    this->libraryMenu->addAction("Refresh Library", this, SLOT(refreshLibrary()), QKeySequence::fromString("Ctrl+R"));

    this->markMenu = new QMenu(this);
    this->markMenu->setTitle("Mark Item");
    this->markMenu->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "As Used", this, SLOT(markItemAsUsed()));
    this->markMenu->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "As Unused", this, SLOT(markItemAsUnused()));
    this->markMenu->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "All as Used", this, SLOT(markAllItemsAsUsed()));
    this->markMenu->addAction(/*QIcon(":/Graphics/Images/RenameRundown.png"),*/ "All as Unused", this, SLOT(markAllItemsAsUnused()));

    this->rundownMenu = new QMenu(this);
    this->rundownMenu->addMenu(this->markMenu);
    this->rundownMenu->addSeparator();
    this->rundownMenu->addAction("Copy Item Properties", this, SLOT(copyItemProperties()), QKeySequence::fromString("Shift+C"));
    this->rundownMenu->addAction("Paste Item Properties", this, SLOT(pasteItemProperties()), QKeySequence::fromString("Shift+V"));
    this->rundownMenu->addSeparator();
    this->compactViewAction = this->rundownMenu->addAction("Compact View");
    this->compactViewAction->setCheckable(true);
    this->allowRemoteTriggeringAction = this->rundownMenu->addAction("Allow Remote Triggering");
    this->allowRemoteTriggeringAction->setCheckable(true);
    this->rundownMenu->addSeparator();
    this->insertRepositoryChangesAction = this->rundownMenu->addAction("Insert Repository Changes", this, SLOT(insertRepositoryChanges()), QKeySequence::fromString("Ins"));
    this->insertRepositoryChangesAction->setEnabled(false);
    this->rundownMenu->addSeparator();
    this->reloadRundownAction = this->rundownMenu->addAction("Reload Rundown", this, SLOT(reloadRundown()), QKeySequence::fromString("Ctrl+L"));
    this->switchRundown1Action = this->rundownMenu->addAction("Switch to Rundown 1", this, SLOT(switchRundown1()), QKeySequence::fromString("Ctrl+Shift+1"));
    this->switchRundown2Action = this->rundownMenu->addAction("Switch to Rundown 2", this, SLOT(switchRundown2()), QKeySequence::fromString("Ctrl+Shift+2"));
    this->rundownMenu->addSeparator();
    this->rundownMenu->addAction("Close Rundown", this, SLOT(closeRundown()), QKeySequence::fromString("Ctrl+W"));

    QObject::connect(this->compactViewAction, SIGNAL(toggled(bool)), this, SLOT(compactView(bool)));
    QObject::connect(this->allowRemoteTriggeringAction, SIGNAL(toggled(bool)), this, SLOT(allowRemoteTriggering(bool)));

    this->playoutMenu = new QMenu(this);
    this->playoutMenu->addAction("Stop", this, SLOT(executeStop()), QKeySequence::fromString("F1"));
    this->playoutMenu->addAction("Play", this, SLOT(executePlay()), QKeySequence::fromString("F2"));
    this->playoutMenu->addAction("Load", this, SLOT(executeLoad()), QKeySequence::fromString("F3"));
    this->playoutMenu->addAction("Pause / Resume", this, SLOT(executePause()), QKeySequence::fromString("F4"));
    this->playoutMenu->addSeparator();
    this->playoutMenu->addAction("Next", this, SLOT(executeNext()), QKeySequence::fromString("F5"));
    this->playoutMenu->addAction("Update", this, SLOT(executeUpdate()), QKeySequence::fromString("F6"));
    this->playoutMenu->addAction("Invoke", this, SLOT(executeInvoke()), QKeySequence::fromString("F7"));
    this->playoutMenu->addAction("Preview", this, SLOT(executePreview()), QKeySequence::fromString("F8"));
    this->playoutMenu->addSeparator();
    this->playoutMenu->addAction("Clear", this, SLOT(executeClear()), QKeySequence::fromString("F10"));
    this->playoutMenu->addAction("Clear Video Layer", this, SLOT(executeClearVideolayer()), QKeySequence::fromString("F11"));
    this->playoutMenu->addAction("Clear Channel", this, SLOT(executeClearChannel()), QKeySequence::fromString("F12"));
    this->playoutMenu->addSeparator();
    this->playoutMenu->addAction("Play Now", this, SLOT(executePlayNow()), QKeySequence::fromString("Shift+F2"));

    this->helpMenu = new QMenu(this);
    QAction* action = this->helpMenu->addAction("View Help", this, SLOT(showHelpDialog()), QKeySequence::fromString("Ctrl+H"));
    this->helpMenu->addSeparator();
    this->helpMenu->addAction("About CasparCG Client...", this, SLOT(showAboutDialog()));
    action->setEnabled(false);

    this->menuBar = new QMenuBar(this);
    this->menuBar->addMenu(this->fileMenu)->setText("File");
    this->menuBar->addMenu(this->editMenu)->setText("Edit");
    this->menuBar->addMenu(this->viewMenu)->setText("View");
    this->menuBar->addMenu(this->libraryMenu)->setText("Library");
    this->menuBar->addMenu(this->rundownMenu)->setText("Rundown");
    this->menuBar->addMenu(this->playoutMenu)->setText("Playout");
    this->menuBar->addMenu(this->helpMenu)->setText("Help");

    setMenuBar(this->menuBar);
}

void MainWindow::openRecentMenuHovered()
{
    foreach (QAction* action, this->openRecentMenu->actions())
        this->openRecentMenu->removeAction(action);

    QList<QString> paths = DatabaseManager::getInstance().getOpenRecent();
    foreach (QString path, paths)
        this->openRecentMenu->addAction(/*QIcon(":/Graphics/Images/OpenRecent.png"),*/ path);

    if (this->openRecentMenu->actions().count() > 0)
    {
        this->openRecentMenu->addSeparator();
        this->openRecentMenu->addAction(/*QIcon(":/Graphics/Images/ClearOpenRecent.png"),*/ "Clear Menu", this, SLOT(clearOpenRecent()));
    }
}

void MainWindow::openRecentMenuActionTriggered(QAction* action)
{
    if (action->text().contains("Clear"))
        return;

    EventManager::getInstance().fireOpenRundownEvent(OpenRundownEvent(action->text()));
}

void MainWindow::clearOpenRecent()
{
   DatabaseManager::getInstance().deleteOpenRecent();
}

void MainWindow::reloadRundownMenu(const ReloadRundownMenuEvent& event)
{
    this->reloadRundownAction->setEnabled(event.getEnabled());
}

void MainWindow::switchRundown1Menu(const SwitchRundown1MenuEvent& event)
{
    this->switchRundown1Action->setEnabled(event.getEnabled());
}

void MainWindow::switchRundown2Menu(const SwitchRundown2MenuEvent& event)
{
    this->switchRundown2Action->setEnabled(event.getEnabled());
}

void MainWindow::emptyRundown(const EmptyRundownEvent& event)
{
    Q_UNUSED(event);

    this->saveAsPresetAction->setEnabled(false);
}

void MainWindow::statusbar(const StatusbarEvent& event)
{
    if (this->statusBar()->currentMessage() == event.getMessage())
        return;

    this->statusBar()->showMessage(event.getMessage(), event.getTimeout());
}

void MainWindow::activeRundownChanged(const ActiveRundownChangedEvent& event)
{
    QFileInfo info(event.getPath());
    if (info.baseName() == Rundown::DEFAULT_NAME)
        this->setWindowTitle(QString("%1").arg(this->applicationTitle));
    else
        this->setWindowTitle(QString("%1 - %2").arg(this->applicationTitle).arg(event.getPath()));
}

void MainWindow::newRundownMenu(const NewRundownMenuEvent& event)
{
    this->newRundownAction->setEnabled(event.getEnabled());
}

void MainWindow::openRundownMenu(const OpenRundownMenuEvent& event)
{
    this->openRundownAction->setEnabled(event.getEnabled());
}

void MainWindow::exportPresetMenu(const ExportPresetMenuEvent& event)
{
    this->exportPresetAction->setEnabled(event.getEnabled());
}

void MainWindow::saveAsPresetMenu(const SaveAsPresetMenuEvent& event)
{
    this->saveAsPresetAction->setEnabled(event.getEnabled());
}

void MainWindow::openRundownFromUrlMenu(const OpenRundownFromUrlMenuEvent& event)
{
    this->openRundownFromUrlAction->setEnabled(event.getEnabled());
}

void MainWindow::allowRemoteTriggering(const AllowRemoteTriggeringEvent& event)
{
    // We do not want to trigger check changed event.
    this->allowRemoteTriggeringAction->blockSignals(true);
    this->allowRemoteTriggeringAction->setChecked(event.getEnabled());
    this->allowRemoteTriggeringAction->blockSignals(false);
}

void MainWindow::compactView(const CompactViewEvent& event)
{
    // We do not want to trigger check changed event.
    this->compactViewAction->blockSignals(true);
    this->compactViewAction->setChecked(event.getEnabled());
    this->compactViewAction->blockSignals(false);
}

void MainWindow::repositoryRundown(const RepositoryRundownEvent& event)
{
    this->saveAction->setEnabled(!event.getRepositoryRundown());
    this->saveAsAction->setEnabled(!event.getRepositoryRundown());
    this->allowRemoteTriggeringAction->setEnabled(!event.getRepositoryRundown());
    this->insertRepositoryChangesAction->setEnabled(event.getRepositoryRundown());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (this->widgetRundown->checkForSaveBeforeQuit())
    {
        EventManager::getInstance().fireCloseApplicationEvent(CloseApplicationEvent());

        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::importPreset()
{
    EventManager::getInstance().fireImportPresetEvent(ImportPresetEvent());
}

void MainWindow::exportPreset()
{
    EventManager::getInstance().fireExportPresetEvent(ExportPresetEvent());
}

void MainWindow::saveAsPreset()
{
    EventManager::getInstance().fireSaveAsPresetEvent(SaveAsPresetEvent());
}

void MainWindow::newRundown()
{
    EventManager::getInstance().fireNewRundownEvent(NewRundownEvent());
}

void MainWindow::openRundown()
{
    EventManager::getInstance().fireOpenRundownEvent(OpenRundownEvent());
}

void MainWindow::openRundownFromUrl()
{
    EventManager::getInstance().fireOpenRundownFromUrlEvent(OpenRundownFromUrlEvent());
}

void MainWindow::saveRundown()
{
    EventManager::getInstance().fireSaveRundownEvent(SaveRundownEvent(false));
}

void MainWindow::saveAsRundown()
{
    EventManager::getInstance().fireSaveRundownEvent(SaveRundownEvent(true));
}

void MainWindow::copyItemProperties()
{
    EventManager::getInstance().fireCopyItemPropertiesEvent(CopyItemPropertiesEvent());
}

void MainWindow::pasteItemProperties()
{
    EventManager::getInstance().firePasteItemPropertiesEvent(PasteItemPropertiesEvent());
}

void MainWindow::executeStop()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F1, Qt::NoModifier));
}

void MainWindow::executePlay()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F2, Qt::NoModifier));
}

void MainWindow::executePlayNow()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F2, Qt::ShiftModifier));
}

void MainWindow::executeLoad()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F3, Qt::NoModifier));
}

void MainWindow::executePause()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F4, Qt::NoModifier));
}

void MainWindow::executeNext()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F5, Qt::NoModifier));
}

void MainWindow::executeUpdate()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F6, Qt::NoModifier));
}

void MainWindow::executeInvoke()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F7, Qt::NoModifier));
}

void MainWindow::executePreview()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F8, Qt::NoModifier));
}

void MainWindow::executeClear()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F10, Qt::NoModifier));
}

void MainWindow::executeClearVideolayer()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F11, Qt::NoModifier));
}

void MainWindow::executeClearChannel()
{
    EventManager::getInstance().fireExecutePlayoutCommandEvent(ExecutePlayoutCommandEvent(QEvent::KeyPress, Qt::Key_F12, Qt::NoModifier));
}

void MainWindow::markItemAsUsed()
{
    EventManager::getInstance().fireMarkItemAsUsedEvent(MarkItemAsUsedEvent());
}

void MainWindow::markItemAsUnused()
{
    EventManager::getInstance().fireMarkItemAsUnusedEvent(MarkItemAsUnusedEvent());
}

void MainWindow::markAllItemsAsUsed()
{
    EventManager::getInstance().fireMarkAllItemsAsUsedEvent(MarkAllItemsAsUsedEvent());
}

void MainWindow::markAllItemsAsUnused()
{
    EventManager::getInstance().fireMarkAllItemsAsUnusedEvent(MarkAllItemsAsUnusedEvent());
}

void MainWindow::compactView(bool enabled)
{
    EventManager::getInstance().fireCompactViewEvent(CompactViewEvent(enabled));
}

void MainWindow::allowRemoteTriggering(bool enabled)
{
    EventManager::getInstance().fireAllowRemoteTriggeringEvent(AllowRemoteTriggeringEvent(enabled));
}

void MainWindow::closeRundown()
{
    EventManager::getInstance().fireCloseRundownEvent(CloseRundownEvent());
}

void MainWindow::insertRepositoryChanges()
{
    EventManager::getInstance().fireInsertRepositoryChangesEvent(InsertRepositoryChangesEvent());
}

void MainWindow::reloadRundown()
{
    EventManager::getInstance().fireReloadRundownEvent(ReloadRundownEvent());
}

void MainWindow::switchRundown1()
{
    EventManager::getInstance().fireSwitchRundown1Event(SwitchRundown1Event());
}

void MainWindow::switchRundown2()
{
    EventManager::getInstance().fireSwitchRundown2Event(SwitchRundown2Event());
}

void MainWindow::showAboutDialog()
{
    AboutDialog* dialog = new AboutDialog(this);
    dialog->exec();
}

void MainWindow::showHelpDialog()
{
    HelpDialog* dialog = new HelpDialog(this);
    dialog->exec();
}

void MainWindow::showSettingsDialog()
{
    // Reset inspector panel.
    EventManager::getInstance().fireEmptyRundownEvent(EmptyRundownEvent());

    SettingsDialog* dialog = new SettingsDialog(this);
    QObject::connect(dialog, SIGNAL(gpiBindingChanged(int, Playout::PlayoutType)), this->widgetRundown, SLOT(gpiBindingChanged(int, Playout::PlayoutType)));

    dialog->exec();
}

void MainWindow::toggleFullscreen()
{
    isFullScreen() ? setWindowState(Qt::WindowNoState) : setWindowState(Qt::WindowFullScreen);

    EventManager::getInstance().fireToggleFullscreenEvent(ToggleFullscreenEvent());
}

void MainWindow::refreshLibrary()
{
    EventManager::getInstance().fireRefreshLibraryEvent(RefreshLibraryEvent());
}
