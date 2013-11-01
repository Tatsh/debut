#ifndef DEBUTFORM_H
#define DEBUTFORM_H

#include <QDir>
#include "ui_debutform.h"

#define TEXTLIMIT 1024

#define MP3 0
#define OGG 1

class DebutForm : public QWidget, private Ui::DebutForm {
  Q_OBJECT

  public:
    DebutForm(QWidget *parent = 0);
    void loadNPSong();
    void prevFilename(QString tracknumber, QString artist, QString song, int filetype);
    //void prevFilename(QString tracknumber, QString artist, QString song, QString group, int filetype);
    void rename(QString src, QString dest);

  private slots:
    // Outside widgets
    void on_buttonQuit_clicked();
    void on_buttonPrefs_clicked();

    // Album widgets
    void on_buttonAlbumDir_clicked();
    void on_buttonAlbumReset_clicked();
    void on_buttonAlbumApply_clicked();
    void on_textAlbumArtist_textChanged();
    void on_textGroup_textChanged();
    void on_textSongSong_textChanged();
    void on_textSongTrackNum_textChanged();
    void on_textSongArtist_textChanged();

    // Song widgets
    void on_buttonSongNext_clicked();
    void on_buttonSongPrev_clicked();

  private:
    bool folder;

    int index;

    // Initial values when directory is opened
    QString initialArtist;
    QString initialAlbum;
    int initialDate;
    int initialGenreIndex;
    QString initialComment;

    QString currentFile;
    QString group;
    QString nTracks;
    QString path;
    QStringList mp3s;

    QStringList findFiles(const QDir &directory, const QStringList &files);
};

#endif
