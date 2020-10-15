#pragma once
#ifndef _BYTE_ARRAY_VECTOR_H_
#define _BYTE_ARRAY_VECTOR_H_

#include <QByteArray>
#include <QVector>
#include <Tools/date_time_ll.h>
#include <NetLinkData/header_udp_stream.h>

namespace Datalink
{

    typedef QVector < QByteArray > QByteArrays;

    class ByteArray : public QByteArray
    {

    public:

        ByteArray ();

        ~ByteArray ();
        /*!
         * \brief setSize
         * Установка размера буфера
         */
        void setSize ( int );

        int usefulSize () const;
        /*!
         * \brief setUuid
         * Установить текущий ключ
         */
        void setUuid ( const QUuid & );
        /*!
         * \brief currentUuid
         * Текущий Uuid
         * \return
         */
        QUuid currentUuid () const;
        /*!
         * \brief complete
         * Признак завершенного приема
         * \return
         */
        bool complete() const;
        /*!
         * \brief setCompleteFlag
         * Установка признака завершения процесса получения данных
         */
        void setCompleteFlag ( bool );

        DateTimeLL lastReceiveMessage () const;

        void resetLastReceiveMessage ();
        /*!
         * \brief olderThan
         * Последнее сообщение старше, чем msec
         * \return
         */
        bool olderThan ( qint64 msec ) const;
        /*!
         * \brief resetPacketCounter
         * Сброс счетчика пакетов с полезной информацией
         */
        void resetPacketCounter ();
        /*!
         * \brief increasepacketCounter
         * Увеличение на 1 счетчика принятых пакетов
         */
        void increasePacketCounter ();
        /*!
         * \brief fullPacketReceive
         * Принятие решение о приеме всех пакетов( true )
         * \return
         */
        bool fullPacketReceive ( int );

    private:
        /*!
         * \brief m_useful_size
         * Размер полезной части
         */
        int m_useful_size;
        /*!
         * \brief m_uuid
         * Текущий ключ сообщения
         */
        QUuid m_uuid;
        /*!
         * \brief m_complete_flag
         * Флаг завершения приема всех пакетов
         */
        bool m_complete_flag;
        /*!
         * \brief m_dt_last_receive_message
         * Время последней записи
         */
        DateTimeLL m_dt_last_receive_message;
        /*!
         * \brief m_count_packet
         * Количество принятых пакетов
         */
        int m_count_packet;
    };

    /*!
     * \brief The ByteArrayVector class
     * Класс предназначен для управления конвейром сообщений
     * Представляет собой квази двумерный массив ( в каждой ячейке НЕ равные буфера )
     */
    class ByteArrayVector : protected QVector < ByteArray >
	{

	public:

        /*!
         * \brief ByteArrayVector
         * n - количество буферов
         */
        ByteArrayVector ( int n );
        /*!
        *
        */
        ~ByteArrayVector ();
        /*!
         * \brief setStartReceive
         * Организация начала приема:
         * Поиск свободного буфера, установка размеров...
         */
        void setStartReceive ( const UdpStreamHeader & header );
        /*!
         * \brief saveUsefulMessage
         * \param header
         */
        void saveUsefulMessage ( const UdpStreamHeader & header
                               , const QByteArray & );
        /*!
         * \brief completeReceive
         * \param header
         */
        void completeReceive ( const UdpStreamHeader & header );
        /*!
         * \brief completeBuffers
         * Возврат завершенных приемов
         * \return
         */
        QByteArrays completeBuffers ();
        /*!
         * \brief completeCount
         * Количество завершенных приемов
         * \return
         */
        int completeCount () const;

    protected:
        /*!
         * \brief getFreeCell
         * \return
         */
        int getFreeCell ( const QUuid & uuid_p ) const;

    private:
        /*!
         * \brief sem_stream_process
         */
       // QSemaphore sem_stream_process;

    }; // class ByteArrayVector
} // namespace Datalink

#endif // _BYTE_ARRAY_VECTOR_H_

