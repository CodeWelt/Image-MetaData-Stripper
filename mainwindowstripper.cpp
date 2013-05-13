#include "mainwindowstripper.h"
#include "ui_mainwindowstripper.h"
#include <QtGui>
#include <QImage>

MainWindowStripper::MainWindowStripper(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowStripper) {
    ui->setupUi(this);
}

MainWindowStripper::~MainWindowStripper() {
    delete ui;
}

void MainWindowStripper::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindowStripper::dropEvent(QDropEvent *event) {
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        ui->statusBar->showMessage(tr("No URLs dropped. Idle."));
        return;
    }
    QStringList fileNameList;
    for (int i = 0; i < urls.length(); i++) {
        QString fileName = urls.at(i).toLocalFile();
        if (!fileName.isEmpty() && QFile::exists(fileName)) fileNameList << fileName;
    }
    if (!fileNameList.isEmpty()) process(fileNameList);
    else ui->statusBar->showMessage(tr("No Files dropped. Idle."));

}

void MainWindowStripper::on_openFileButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(trUtf8("Image Files (*.*)"));
    dialog.setWindowTitle("Please choose image files to strip meta");
    QStringList fileNames;
    QStringList processThese;
    if (dialog.exec())
    fileNames = dialog.selectedFiles();
    for (int i = 0; i < fileNames.length(); i++) {
        QString fileName = fileNames.at(i);
        if (!fileName.isEmpty() && QFile::exists(fileName)) processThese << fileName;
    }
    if (!processThese.isEmpty()) process(processThese);
    else ui->statusBar->showMessage(tr("Please open image Files. Idle."));
}

void MainWindowStripper::process(QStringList fileNameList) {
    ui->clear->setEnabled(true);
    qint64 totalStrippedSize = 0;
    QString log;
    QString more("");
    if (fileNameList.length() != 1) more = tr("s");
    ui->log->appendHtml(log.sprintf("<font size=\"3\">%1</font> File%2 opened.\n").arg(fileNameList.length()).arg(more));
    ui->statusBar->showMessage(tr("Working ..."));
    for (int i = 0; i < fileNameList.length(); i++) {
        QImage sourceImage(fileNameList.at(i));
        if (sourceImage.isNull()) {
            ui->log->appendHtml(log.sprintf("<font size=\"4\" color=\"#FF0000\">Error loading %1. This is not an image file. Skipping ...</font>\n").arg(fileNameList.at(i)));
        } else {
            ui->log->appendHtml(log.sprintf("<font size=\"3\" color=\"#666666\">Processing %1 ...</font>\n").arg(fileNameList.at(i)));
            QString formatToString;
            switch (sourceImage.format()) {
            case 0:
             formatToString = tr("Invalid Format");
             break;
            case 1:
             formatToString = tr("Mono");
             break;
            case 2:
             formatToString = tr("MonoLSB");
             break;
            case 3:
             formatToString = tr("Indexed8");
             break;
            case 4:
             formatToString = tr("RGB32");
             break;
            case 5:
             formatToString = tr("ARGB32");
             break;
            case 6:
             formatToString = tr("ARGB32 Premultiplied");
             break;
            case 7:
             formatToString = tr("RGB16");
             break;
            case 8:
             formatToString = tr("ARGB8565 Premultiplied");
             break;
            case 9:
             formatToString = tr("RGB666");
             break;
            case 10:
             formatToString = tr("ARGB6666 Premultiplied");
             break;
            case 11:
             formatToString = tr("RGB555");
             break;
            case 12:
             formatToString = tr("ARGB8555 Premultiplied");
             break;
            case 13:
             formatToString = tr("RGB888");
             break;
            case 14:
             formatToString = tr("RGB444");
             break;
            case 15:
             formatToString = tr("ARGB4444 Premultiplied");
             break;
            default:
             formatToString = tr("Invalid Format");
            }

            QString alphaToString = "No alpha channel found";
            if (sourceImage.hasAlphaChannel()) alphaToString = "This image has an alpha channel";
            QImage target(QSize(sourceImage.width(), sourceImage.height()), sourceImage.format());
            for (int x = 0; x < sourceImage.width(); x++) {
                for (int y = 0; y < sourceImage.height(); y++) {
                    target.setPixel(x, y, sourceImage.pixel(x, y));
                }
            }

            QString stringBak = fileNameList.at(i);
            while (QFile::exists(stringBak)) {
                stringBak.append(tr(".BAK"));
            }
            QFile::copy(fileNameList.at(i), stringBak);

            target.save(fileNameList.at(i));
            QFileInfo sourceInfo(stringBak);
            QFileInfo targetInfo(fileNameList.at(i));

            qint64 sourceSize = sourceInfo.size();
            qint64 targetSize = targetInfo.size();

            if (!ui->backupCheckBox->isChecked()) QFile::remove(stringBak);

            if (targetSize < sourceSize){
                totalStrippedSize += sourceSize - targetSize;
                ui->log->appendHtml(log.sprintf("<font size=\"4\" color=\"#009900\">%1 Bytes Removed! - Details - Image Width: %2; Image Height: %3; Image Format: %4; %5</font>\n").arg((int)(sourceSize - targetSize)).arg(sourceImage.width()).arg(sourceImage.height()).arg(formatToString).arg(alphaToString));
            } else ui->log->appendHtml(log.sprintf("<font size=\"4\" color=\"#666666\">Nothing stripped. - Details - Image Width: %1; Image Height: %2; Image Format: %3; %4</font>\n").arg(sourceImage.width()).arg(sourceImage.height()).arg(formatToString).arg(alphaToString));
        }
    }
    ui->statusBar->showMessage(tr("Done."));
    if (totalStrippedSize > 0) {
        QString totalStrippedSizeToString;
        if (totalStrippedSize < 1024) totalStrippedSizeToString.sprintf("%1 Bytes").arg(totalStrippedSize);
        else if (totalStrippedSize < 1048576) totalStrippedSizeToString.sprintf("%1 KB").arg(totalStrippedSize/1024);
        else if (totalStrippedSize < 1073841824) totalStrippedSizeToString.sprintf("%1 MB").arg(totalStrippedSize/1024/1024);
        else totalStrippedSizeToString.sprintf("%1 GB").arg(totalStrippedSize/1024/1024/1024);
        ui->log->appendHtml(log.sprintf("<font size=\"5\" color=\"#009900\">A Total of %1 Bytes were Stripped!</font>\n").arg(totalStrippedSize));
    } else ui->log->appendHtml(log.sprintf("<font size=\"5\" color=\"#0000000\">Nothing to do. The input does not contain MetaData.</font>\n"));
    if (ui->quitCheckBox->isChecked()) qApp->quit();
}

void MainWindowStripper::on_clear_clicked()
{
    ui->log->clear();
}
