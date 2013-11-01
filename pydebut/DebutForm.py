import eyeD3
import os

from DebutFormUi import Ui_DebutForm
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import QDir, QStringList, QString, Qt
from PyQt4.QtGui import QFileDialog, QMessageBox

class DebutForm(QtGui.QWidget):
    FILE_TYPE_MP3 = 0

    # QString/str/None
    current_file = None
    initial_artist = None
    initial_album = None
    initial_year = None
    initial_comment = ''
    group = 'iND'
    path = None

    # boolean
    directory = False

    # integer
    initial_genre_index = -1
    index = 0
    number_of_tracks = 0

    # list, etc
    mp3s = []

    def __init__(self):
        QtGui.QWidget.__init__(self)

        self.ui = Ui_DebutForm()

        self.ui.setupUi(self)

        eyeD3.utils.STRICT_ID3 = 0


    # ... button
    def on_buttonAlbumDir_pressed(self):
        self.directory = False

        search = u'*.mp3'
        self.path = QFileDialog.getExistingDirectory(self, self.tr('Choose album directory'), QDir.currentPath(), QFileDialog.ShowDirsOnly)
        directory = QDir(self.path)

        if directory.dirName() != '.': # `.` usually means cancel was pressed
            self.ui.textAlbumFolder.setText(directory.dirName())

        self.mp3s = directory.entryList(QStringList(search), QDir.Files)

        if len(self.mp3s) == 0:
            QMessageBox.information(self, self.tr('No MP3 files found'), self.tr('No MP3 files found. Select a directory with MP3 files.'))
            self.ui.textOutOfNumTracks.setText(' / 0')
            return
        else:
            display_fn = self.mp3s[0]
            self.current_file = os.path.join(unicode(self.path.toUtf8()), unicode(display_fn.toUtf8()))
            raw_path = str(self.current_file.toUtf8())

            self.ui.textSongFilename.setText(display_fn)

            # Enable buttons
            self.ui.buttonAlbumApply.setEnabled(True)
            self.ui.buttonAlbumReset.setEnabled(True)
            self.ui.buttonSongReset.setEnabled(True)
            self.ui.buttonSongNext.setEnabled(True)

            tag = eyeD3.Tag()

            if tag.link(raw_path) is not False:
                # Store initial values
                self.initial_artist = tag.getArtist()
                self.initial_album = tag.getAlbum()
                self.initial_year = tag.getYear()
                genre = tag.getGenre()

                self.ui.textAlbumArtist.setText(self.initial_artist)
                self.ui.textAlbumAlbum.setText(self.initial_album)
                self.ui.textAlbumDate.setText(self.initial_year)

                if genre is not None:
                    self.initial_genre_index = self.ui.comboAlbumGenre.findText(genre.getName(), Qt.MatchExactly)
                    end_offset = self.ui.comboAlbumGenre.count() + 1

                    if self.initial_genre_index == -1:
                        self.ui.comboAlbumGenre.insertItem(end_offset, genre.getName())
                        self.ui.comboAlbumGenre.setCurrentIndex(end_offset - 1)
                        self.initial_genre_index = end_offset - 1
                    else:
                        self.ui.comboAlbumGenre.setCurrentIndex(genre_index)
                        self.initial_genre_index = genre_index

                # Not supporting multiple comment frames so they just get joined
                join_with = '\n'
                comments = tag.getComments()

                self.initial_comment = join_with.join([unicode(x) for x in comments])
                self.ui.textAlbumComment.setText(self.initial_comment)

                # Specific to the file
                self.ui.textSongSong.setText(tag.getTitle())
                self.ui.textSongArtist.setText(tag.getArtist())
                self.ui.textSongTrackNum.setText(str(tag.getTrackNum()[0]))

        self.number_of_tracks = ' / %d' % len(self.mp3s)
        self.index = 0
        self.directory = True

        self.ui.textOutOfNumTracks.setText(self.number_of_tracks)

    def _previewFilename(self, track_number, artist, song, file_type):
        new_name = '%1-%2-%3-%4.mp3'

        # if file_type == MP3:
        if track_number.toInt() < 10:
            new_name = '0' + new_name

        new_name = QString(new_name).arg(track_number).arg(artist).arg(song).arg(self.ui.textGroup.text())
        new_name = new_name.toLower()
        new_name = new_name.replace(' ', '_')

        self.ui.textSongFilename.setText(new_name)

    def _generalTextChangedEventHandler(self):
        self._previewFilename(self.ui.textSongTrackNum.text(), self.ui.textAlbumArtist.text(), self.ui.textSongSong.text(), self.FILE_TYPE_MP3)

    def on_textAlbumArtist_textChanged(self):
        self._generalTextChangedEventHandler()

    def on_textGroup_textChanged(self):
        self._generalTextChangedEventHandler()

    def on_textSongSong_textChanged(self):
        self._generalTextChangedEventHandler()

    def on_textSongTrackNum_textChanged(self):
        self._generalTextChangedEventHandler()

    def on_textSongArtist_textChanged(self):
        self._generalTextChangedEventHandler()

    def _getFullPathToCurrentSong(self):
        prefix = unicode(self.path.toUtf8())
        mp3 = self.mp3s[self.index]
        return os.path.join(prefix, unicode(mp3.toUtf8()))

    def on_buttonAlbumApply_pressed(self):
        if self.directory is False:
            return

        prefix = unicode(self.path.toUtf8())

        for mp3 in self.mp3s:
            full_path = self._getFullPathToCurrentSong()

            tag = eyeD3.Tag()
            tag.link(full_path)

            if self.ui.textAlbumArtist.isModified():
                tag.setArtist(self.ui.textAlbumArtist.text())
                self.ui.textAlbumArtist.setModified(False)

            if self.ui.textAlbumAlbum.isModified():
                tag.setAlbum(self.ui.textAlbumAlbum.text())
                self.ui.textAlbumAlbum.setModified(False)

            if self.ui.textAlbumDate.isModified():
                tag.setDate(self.ui.textAlbumDate.text())
                self.ui.textAlbumDate.setModified(False)

            if self.ui.comboAlbumGenre.currentIndex() != self.initial_genre_index:
                tag.setGenre(self.ui.comboAlbumGenre.currentText())

            if self.ui.textAlbumComment.document().isModified():
                tag.removeComments()
                tag.addComment(self.ui.textAlbumComment.document.toPlainText())
                self.ui.textAlbumComment.setModified(False)

            # tag.update()

    def on_buttonAlbumReset_pressed(self):
        return

    def _rename(self, src, dest):
        prefix = unicode(self.path.toUtf8())
        src = QString(os.path.join(unicode(src.toUtf8()), prefix))
        dest = QString(os.path.join(unicode(dest.toUtf8()), prefix))

        f = QFile(src)
        return f.rename(dest)

    def _saveChangesCurrentToSong(self):
        current_mp3 = self.mp3s[self.index]
        modified = False
        full_path = self._getFullPathToCurrentSong()
        tag = eyeD3.Tag()
        tag.link(full_path)

        if self.ui.textSongSong.isModified():
            modified = True
            tag.setTitle(self.ui.textSongSong.text())
            self.ui.textSongSong.setModified(False)

        if self.ui.textSongTrackNum.isModified():
            modified = True
            tag.setTrackNum(self.ui.textSongTrackNum.text().toInt(), False) # no zero-padding
            self.ui.textSongTrackNum.setModified(False)

        if self.ui.textSongArtist.isModified():
            modified = True
            tag.setArtist(self.ui.textSongArtist.text())
            self.ui.textSongArtist.setModified(False)

        if self.ui.comboSongGenre != self.initial_genre_index:
            modified = True
            tag.setGenre(self.ui.comboSongGenre.currentText())

        if modified:
            # tag.update()
            pass

        if index == 0:
            self.ui.buttonSongPrev.setEnabled(False)
        elif index == 1:
            self.ui.buttonSongPrev.setText('App&ly')
            self.mp3s.replace(self.index, self.ui.textSongFilename.text())
            self.index = self.index - 1
            self._rename(src, self.ui.textSongFilename.text())
        elif index > 0:
            self.ui.buttonSongNext.setText('&Next >')
            self.ui.buttonSongNext.setEnabled(True)
            self.mp3s.replace(self.index, self.ui.textSongFilename.text())
            self._rename(src, self.ui.textSongFilename.text())
            self.index = self.index - 1

    def loadPreviousSong(self):
        return

    def loadNextSong(self):
        return

    def on_buttonSongPrev_pressed(self):
        _saveChangesCurrentToSong()

        if self.index < len(self.mp3s):
            self.ui.buttonSongPrev.setEnabled(True)

        if index > 0:
            loadPreviousSong()

    def on_buttonSongNext_pressed(self):
        _saveChangesCurrentToSong()

        if (index + 1) < len(self.mp3s) - 1:
            loadNextSong()
