#include <QtGui>
#include <taglib/mpegfile.h>
#include <taglib/apetag.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/tstring.h>
#include <iostream>
#include <cstdio>
#include "debutform.h"
#include "prefsdialog.h"

using namespace std;
using namespace TagLib::MPEG;

/**
  * @param parent - first QWidget
  * @return DebutForm widget
  */
DebutForm::DebutForm(QWidget *parent) : QWidget(parent) {
  folder = false;
  setupUi(this);
}

/**
  * User clicks '...' (browse) button
  */
void DebutForm::on_buttonAlbumDir_clicked() {
  QString search = "*.mp3";
  QDir directory;

  path = QFileDialog::getExistingDirectory(
    this,
    tr("Choose album folder"),
    QDir::currentPath(),
    QFileDialog::ShowDirsOnly
  );

  folder = true; // Yes, we got a directory
  directory = QDir(path);

  textAlbumFolder->setText(directory.dirName()); // Set text to relative path, not full path

  mp3s = directory.entryList(QStringList(search), QDir::Files);
  if (mp3s.size() == 0) {
    QMessageBox::information(this, tr("No audio files found."),
                             tr("No audio files found. Select a directory with audio files."));
    textOutOfNumTracks->setText(" / 0"); // Reset just in case
    return;
  }
  else {
    QString dispFilename = mp3s.at(0);
    textSongFilename->setText(dispFilename);

    currentFile = dispFilename.prepend("/").prepend(path);
    File *f = new File(currentFile.toUtf8().constData(), true);

    // Enable the buttons
    buttonAlbumApply->setEnabled(true);
    buttonAlbumReset->setEnabled(true);
    buttonSongReset->setEnabled(true);
    buttonSongNext->setEnabled(true);

    // ID3v2 first
    if (f->ID3v2Tag()) {
      // These should be global if editing an album
      textAlbumArtist->setText(TStringToQString(f->ID3v2Tag()->artist()));
      textAlbumAlbum->setText(TStringToQString(f->ID3v2Tag()->album()));
      textAlbumDate->setText(QString::number(f->ID3v2Tag()->year()));

      // Initial values stored
      initialArtist = TStringToQString(f->ID3v2Tag()->artist());
      initialAlbum = TStringToQString(f->ID3v2Tag()->album());
      initialDate = f->ID3v2Tag()->year();

      // Compare genre with list; if there's a match use it, else add a new one to the list
      if (comboAlbumGenre->findText(TStringToQString(f->ID3v2Tag()->genre()), Qt::MatchExactly) != -1) {
        comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(TStringToQString(f->ID3v2Tag()->genre()), Qt::MatchExactly));

        //TODO: Check all files here and compare genres to see if they are the same
        // comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText("Varies", Qt::MatchExactly); for when genres differ between files
      }
      else {
        comboAlbumGenre->addItem(TStringToQString(f->ID3v2Tag()->genre()));
        comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(TStringToQString(f->ID3v2Tag()->genre()), Qt::MatchExactly));
      }
      initialGenreIndex = comboAlbumGenre->currentIndex();

      // Comment on the first file
      textAlbumComment->setText(TStringToQString(f->ID3v2Tag()->comment()));
      initialComment = TStringToQString(f->ID3v2Tag()->comment());

      // Specific file widgets (like title, which is obviously not global)
      textSongSong->setText(TStringToQString(f->ID3v2Tag()->title()));
      textSongTrackNum->setText(QString::number(f->ID3v2Tag()->track()));
      textSongArtist->setText(TStringToQString(f->ID3v2Tag()->artist()));
    }
    else if (f->ID3v1Tag()) {
      textAlbumArtist->setText(TStringToQString(f->ID3v1Tag()->artist()));
      textAlbumAlbum->setText(TStringToQString(f->ID3v1Tag()->album()));
      textAlbumDate->setText(QString::number(f->ID3v1Tag()->year()));
      if (comboAlbumGenre->findText(TStringToQString(f->ID3v1Tag()->genre()), Qt::MatchExactly) != -1) {
        comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(TStringToQString(f->ID3v1Tag()->genre()), Qt::MatchExactly));

        //TODO: Check all files here and compare genres to see if they are the same
        // comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText("Varies", Qt::MatchExactly); for when genres differ between files
      }
      else {
        comboAlbumGenre->addItem(TStringToQString(f->ID3v1Tag()->genre()));
        comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(TStringToQString(f->ID3v1Tag()->genre()), Qt::MatchExactly));
      }
      initialGenreIndex = comboAlbumGenre->currentIndex();

      // Comment on the first file
      textAlbumComment->setText(TStringToQString(f->ID3v1Tag()->comment()));

      // Song data widgets
      textSongSong->setText(TStringToQString(f->ID3v1Tag()->title()));
      textSongTrackNum->setText(QString::number(f->ID3v1Tag()->track()));
      textSongArtist->setText(TStringToQString(f->ID3v1Tag()->artist()));
    }

    // TODO: Bug check, code audit, etc; implement 100%
    /*else if (f->APETag()) { // Read from this if necessary, but right now does not write them; easy to implement though
      textAlbumArtist->setText(TStringToQString(f->APETag()->artist()));
      textAlbumAlbum->setText(TStringToQString(f->APETag()->album()));
      textAlbumDate->setText(QString::number(f->APETag()->year()));
      if (comboAlbumGenre->findText(TStringToQString(f->APETag()->genre()),
          Qt::MatchExactly) != -1) {
            comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(TStringToQString(f->APETag()->genre()),
                                             Qt::MatchExactly)); // This affects Song data's genre, but not vice versa
            // comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText("Varies", Qt::MatchExactly); for when genres differ between files
            albumGenre_0 = songGenre_0 = comboAlbumGenre->currentText();
          }
          else {
            comboAlbumGenre->addItem(TStringToQString(f->APETag()->genre()));
            comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(TStringToQString(f->APETag()->genre()),
                                             Qt::MatchExactly));
            albumGenre_0 = songGenre_0 = comboAlbumGenre->currentText();
          }
          textAlbumComment->setText(TStringToQString(f->APETag()->comment()));

          // Song data widgets
          textSongSong->setText(TStringToQString(f->APETag()->title()));
          textSongTrackNum->setText(QString::number(f->APETag()->track()));
          textSongArtist->setText(TStringToQString(f->APETag()->artist()));
    }*/

    else {
      QMessageBox::information(this, tr("Info"), tr("Found no tags in %1.").arg(currentFile));
      folder = false;
      return;
    }

    nTracks = QString(" / %1").arg(mp3s.size());
    textOutOfNumTracks->setText(nTracks);
    // TODO: Groups!!!!
    //if (checkAlbumGroupAppend->checkState() > 1) {
      prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), MP3);
    //}
    //else {
//      prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), 0);
  //  }
  }

  index = 0;

  return;
}

/**
  * Generate preview file name.
  */
void DebutForm::prevFilename(QString tracknumber, QString artist, QString song, int filetype) {
  QString newname;

  switch (filetype) {
    case MP3:
      if (tracknumber.toInt() < 10) // Add a leading 0
        newname = QString("0%1-%2-%3-%4.mp3").arg(tracknumber).arg(artist).arg(song).arg(group);
      else
        newname = QString("%1-%2-%3-%4.mp3").arg(tracknumber).arg(artist).arg(song).arg(group);
      break;

    /*case OGG:
      if (tracknumber.toInt() < 10) // Add a leading 0
        newname = QString("0%1-%2-%3-%4.ogg").arg(tracknumber).arg(artist).arg(song).arg(group);
      else
        newname = QString("%1-%2-%3-%4.ogg").arg(tracknumber).arg(artist).arg(song).arg(group);
      break;*/

    default:
      break;
  }

  textSongFilename->setText(newname.replace(QString(" "), QString("_")).toLower());

  return;
}

/**
  * Event: Album Artist tag edited.
  * Update Preview file name.
  */
void DebutForm::on_textAlbumArtist_textChanged() {
  /*if (checkAlbumGroupAppend->checkState() > 1)
    prevFilename(textSongTrackNum->text(), artist, textSongSong->text(), textGroup->text(), 0);
  else*/
  prevFilename(textSongTrackNum->text(), textAlbumArtist->text(), textSongSong->text(), 0);
  return;
}

/**
  * Event: Group text edited.
  * Update Preview file name.
  */
void DebutForm::on_textGroup_textChanged() {
  //if (checkAlbumGroupAppend->checkState() > 1)
    prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), MP3);
  return;
}

// TODO: Re-add Group field to all these
void DebutForm::on_textSongSong_textChanged() {
  prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), MP3);
  return;
}

void DebutForm::on_textSongTrackNum_textChanged() {
  prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), MP3);
  return;
}

void DebutForm::on_textSongArtist_textChanged() {
  prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), MP3);
  return;
}

void DebutForm::rename(QString src, QString dest) {
  src.prepend(path);
  dest.prepend(path);

  QFile f(src);
  f.rename(dest);

  // Check for SPECIFIC errors other than exists() later
/*  if (f.error())
    QMessageBox::information(this, tr("Error renaming"),
                             tr("Error renaming %1.").arg(src));*/

  return;
}

/**
  * Event: Apply button (for all files) clicked.
  * Go through each field and see if it was modified.
  * Only modify Artist, Album, Date, and Genre on all files in the list.
  */
void DebutForm::on_buttonAlbumApply_clicked() {
  QString temp;
  QString processFile;

  if (folder) {
    for (int i = 0; i < mp3s.size(); ++i) {
      temp = mp3s.at(i);
      processFile = temp.prepend("/").prepend(path);
      File *f = new File(processFile.toUtf8().constData(), true);
      cout << "Modifying " << processFile.toUtf8().constData() << endl;

      if (textAlbumArtist->isModified()) {
        f->ID3v1Tag()->setArtist(textAlbumArtist->text().toStdWString());
        f->ID3v2Tag()->setArtist(textAlbumArtist->text().toStdWString());
        textAlbumArtist->setModified(false);
      }

      if (textAlbumAlbum->isModified()) {
        f->ID3v1Tag()->setAlbum(textAlbumAlbum->text().toStdWString());
        f->ID3v2Tag()->setAlbum(textAlbumAlbum->text().toStdWString());
        textAlbumAlbum->setModified(false);
      }

      if (textAlbumDate->isModified()) {
        f->ID3v1Tag()->setYear(textAlbumDate->text().toInt());
        f->ID3v2Tag()->setYear(textAlbumDate->text().toInt());
        textAlbumDate->setModified(false);
      }

      if (comboAlbumGenre->currentIndex() != initialGenreIndex) {
        //f->ID3v1Tag()->setGenre(comboAlbumGenre->currentText().toStdWString());
        f->ID3v2Tag()->setGenre(comboAlbumGenre->currentText().toStdWString());
      }

      if (textAlbumComment->document()->isModified()) {
        f->ID3v1Tag()->setComment(textAlbumComment->toPlainText().toStdWString());
        f->ID3v2Tag()->setComment(textAlbumComment->toPlainText().toStdWString()); // Unicode support here?
        textAlbumComment->document()->setModified(false);
      }

      f->save(); // Save even if no changes
    }
  }

  return;
}

/**
  * Event: Button 'Reset' is clicked.
  * Set all values to initial values when directory was opened.
  */
void DebutForm::on_buttonAlbumReset_clicked() {
  return;
  int ret = QMessageBox::No;

  if (checkPromptReset->checkState() > 1) {
    ret = QMessageBox::information(this, tr("Reset album data"),
                tr("Are you sure you want to reset to original values for album data?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
  }

  if (ret == QMessageBox::Yes) {
    TagLib::MPEG::File f(currentFile.toUtf8().constData());

    // Album data widgets
    textAlbumArtist->setText(QString::fromStdWString(f.ID3v2Tag()->artist().toWString()));
    textAlbumAlbum->setText(QString::fromStdWString(f.ID3v2Tag()->album().toWString()));
    textAlbumDate->setText(QString::number(f.ID3v2Tag()->year()));
    comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(QString::fromStdWString(f.ID3v2Tag()->genre().toWString()),
                                     Qt::MatchExactly)); // This affects Song data's genre, but not vice versa
    // comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText("Varies", Qt::MatchExactly);
    textAlbumComment->setText(QString::fromStdWString(f.ID3v2Tag()->comment().toWString()));
  }

  return;
}

/**
  * Modify information, then move to next song
  */
void DebutForm::on_buttonSongNext_clicked() {

  if (index < mp3s.size()) {
    buttonSongPrev->setEnabled(true);
    bool modified = false;
    QString temp = mp3s.at(index);
    QString src = temp;
    QString processFile = temp.prepend(path);
    File *f = new File(processFile.toUtf8().constData(), true);

    if (textSongSong->isModified()) {
      f->ID3v1Tag()->setTitle(textSongSong->text().toStdWString());
      f->ID3v2Tag()->setTitle(textSongSong->text().toStdWString());
      textSongSong->setModified(false);
    }

    if (textSongTrackNum->isModified()) {
      f->ID3v1Tag()->setTrack(textSongTrackNum->text().toInt());
      f->ID3v2Tag()->setTrack(textSongTrackNum->text().toInt());
      textSongTrackNum->setModified(false);
    }

    if (textSongArtist->isModified()) {
      f->ID3v1Tag()->setArtist(textSongArtist->text().toStdWString());
      f->ID3v2Tag()->setArtist(textSongArtist->text().toStdWString());
    }

    if (comboSongGenre->currentIndex() != initialGenreIndex) { // Case sensitive comparison
      f->ID3v1Tag()->setGenre(comboSongGenre->currentText().toStdWString());
      f->ID3v2Tag()->setGenre(comboSongGenre->currentText().toStdWString());
    }

    if (modified)
      f->save();

    if (index == (mp3s.size() - 2)) {
      mp3s.replace(index, textSongFilename->text());
      rename(src, textSongFilename->text());
      index++;
      buttonSongNext->setText("A&pply");
    }
    else if (index == (mp3s.size() - 1))
      buttonSongNext->setEnabled(false);
    else {
      mp3s.replace(index, textSongFilename->text());
      rename(src, textSongFilename->text());
      index++;
      buttonSongPrev->setText("< &Previous");
    }
    loadNPSong();
  }

  return;
}

void DebutForm::on_buttonSongPrev_clicked() {
  if (index < mp3s.size()) {
    buttonSongPrev->setEnabled(true);
    bool modified = false;
    QString temp = mp3s.at(index);
    QString src = temp;
    QString processFile = temp.prepend(path);
    File *f = new File(processFile.toUtf8(), true);

    if (textSongSong->isModified()) {
      if (!modified)
        modified = true;
      f->ID3v1Tag()->setTitle(textSongSong->text().toStdWString());
      f->ID3v2Tag()->setTitle(textSongSong->text().toStdWString());
      textSongSong->setModified(false);
    }

    if (textSongTrackNum->isModified()) {
      if (!modified)
        modified = true;
      f->ID3v1Tag()->setTrack(textSongTrackNum->text().toInt());
      f->ID3v2Tag()->setTrack(textSongTrackNum->text().toInt());
      textSongTrackNum->setModified(false);
    }

    if (textSongArtist->isModified()) {
      if (!modified)
        modified = true;
      f->ID3v1Tag()->setArtist(textSongArtist->text().toStdWString());
      f->ID3v2Tag()->setArtist(textSongArtist->text().toStdWString());
    }

    if (comboSongGenre->currentIndex() != initialGenreIndex) {
      f->ID3v1Tag()->setGenre(comboSongGenre->currentText().toStdWString());
      f->ID3v2Tag()->setGenre(comboSongGenre->currentText().toStdWString());
    }

    if (modified)
      f->save();

    if (index == 0) {
      buttonSongPrev->setEnabled(false);
    }
    else if (index == 1) {
      buttonSongPrev->setText("App&ly");
      mp3s.replace(index, textSongFilename->text());
      rename(src, textSongFilename->text());
      --index;
    }
    else if (index > 0) {
      buttonSongNext->setText("&Next >");
      buttonSongNext->setEnabled(true);
      mp3s.replace(index, textSongFilename->text());
      rename(src, textSongFilename->text());
      --index;
    }
    loadNPSong();
  }

  return;
}

void DebutForm::loadNPSong() {
  QString temp = mp3s.at(index);
  textSongFilename->setText(temp);
  QString processFile = temp.prepend(path);
  File *f = new File(processFile.toUtf8(), true);

  textSongSong->setText(QString::fromStdWString(f->ID3v2Tag()->title().toWString()));
  textSongTrackNum->setText(QString::number(f->ID3v2Tag()->track()));
  if (checkSongArtist->checkState() > 1)
    textSongArtist->setText(QString::fromStdWString(f->ID3v2Tag()->artist().toWString()));
  if (checkSongGenre->checkState() > 1) {
    if (comboAlbumGenre->findText(QString::fromStdWString(f->ID3v2Tag()->genre().toWString()),
        Qt::MatchExactly) != -1) {
          comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(QString::fromStdWString(f->ID3v2Tag()->genre().toWString()),
                                           Qt::MatchExactly)); // This affects Song data's genre, but not vice versa
    // comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText("Varies", Qt::MatchExactly); for when genres differ between files
          //songGenre_0 = comboSongGenre->currentText();
        }
        else {
          comboAlbumGenre->addItem(QString::fromStdWString(f->ID3v2Tag()->genre().toWString()));
          comboAlbumGenre->setCurrentIndex(comboAlbumGenre->findText(QString::fromStdWString(f->ID3v2Tag()->genre().toWString()),
                                           Qt::MatchExactly));
          //songGenre_0 = comboSongGenre->currentText();
        }
  }

  /*if (checkAlbumGroupAppend->checkState() > 1)
    prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), textGroup->text(), 0);
  else*/
    prevFilename(textSongTrackNum->text(), textSongArtist->text(), textSongSong->text(), 0);

  return;
}

void DebutForm::on_buttonPrefs_clicked() {
  PrefsDialog prefs;
  prefs.exec();

  return;
}

void DebutForm::on_buttonQuit_clicked() {
  if (checkPromptQuit->checkState() < 2)
    QCoreApplication::quit();
  else {
    int ret = QMessageBox::information(this, tr("Quit"),
                tr("Are you sure you want to quit?\nAll changes will be lost."),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);

    if (ret == QMessageBox::Yes)
      QCoreApplication::quit();
  }

  return;
}
