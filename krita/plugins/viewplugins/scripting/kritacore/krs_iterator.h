/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KROSS_KRITACOREKRS_ITERATOR_H
#define KROSS_KRITACOREKRS_ITERATOR_H

#include <api/class.h>
#include <api/event.h>

#include <kis_paint_layer.h>
#include <kis_paint_device_impl.h>
#include <kis_types.h>

namespace Kross {

namespace KritaCore {

/**
@author Cyrille Berger
*/
template<class _T_It>
class Iterator : public Kross::Api::Class<Iterator<_T_It> >
{
    public:
    Iterator(_T_It it, KisPaintLayerSP layer) : Kross::Api::Class<Iterator<_T_It> >("KritaIterator"), m_it(it), nchannels(layer->paintDevice()->nChannels()), m_layer(layer)
    {
        // navigate in the iterator
        this->addFunction("next",
            new Kross::Api::ConstFunction0< Iterator<_T_It> >(
                this, &Iterator<_T_It>::next ) );
        this->addFunction("isDone",
            new Kross::Api::ConstFunction0< Iterator<_T_It> >(
                this, &Iterator<_T_It>::isDone ) );

        // get/set value
        QValueVector<KisChannelInfo *> channels = layer->paintDevice()->colorSpace()->channels();
        QString initiales = "";
        for(QValueVector<KisChannelInfo *>::iterator itC = channels.begin(); itC != channels.end(); itC++)
        {
            KisChannelInfo * ci = *itC;
            initiales += ci->name().left(1);
            switch(ci->channelValueType())
            {
                case KisChannelInfo::UINT8:
                    this->addFunction("get"+ci->name(),
                        new Kross::Api::VarFunction1< Iterator<_T_It> , uint >(
                            this, &Iterator<_T_It>::getChannelUINT8, ci->pos() ) );
                    this->addFunction("set"+ci->name(),
                        new Kross::Api::VarFunction1< Iterator<_T_It> , uint >(
                            this, &Iterator<_T_It>::setChannelUINT8, ci->pos() ) );
                    break;
                case KisChannelInfo::UINT16:
                    this->addFunction("get"+ci->name(),
                        new Kross::Api::VarFunction1< Iterator<_T_It> , uint >(
                            this, &Iterator<_T_It>::getChannelUINT16, ci->pos() ) );
                    this->addFunction("set"+ci->name(),
                        new Kross::Api::VarFunction1< Iterator<_T_It> , uint >(
                            this, &Iterator<_T_It>::setChannelUINT16, ci->pos() ) );
                    break;
                case KisChannelInfo::FLOAT32:
                    this->addFunction("get"+ci->name(),
                        new Kross::Api::VarFunction1< Iterator<_T_It> , uint >(
                            this, &Iterator<_T_It>::getChannelFLOAT, ci->pos() ) );
                    this->addFunction("set"+ci->name(),
                        new Kross::Api::VarFunction1< Iterator<_T_It> , uint >(
                            this, &Iterator<_T_It>::setChannelFLOAT, ci->pos() ) );
                    break;
                default:
                    kdDebug() << "unsupported data format in scripts" << endl;
                    break;
            }
        }
        initiales = initiales.upper();
        // set/get general
        addFunction("set" + initiales, &Iterator::setPixel, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::List") );
        addFunction("get" + initiales, &Iterator::getPixel);
        kdDebug() << ( "get" + initiales ) << endl;
        // Various colorSpace
        addFunction("invertColor", &Iterator::invertColor);
        addFunction("darken", &Iterator::darken);
    }

    ~Iterator()
    {
    }
    virtual const QString getClassName() const {
        return "Kross::KritaCore::KrsDoc";
    };
    private:
        Kross::Api::Object::Ptr darken(Kross::Api::List::Ptr args)
        {
            Q_INT32 shade = Kross::Api::Variant::toUInt( args->item(0) );
            bool compensate = Kross::Api::Variant::toBool( args->item(1) );
            double compensation = Kross::Api::Variant::toDouble( args->item(2) );
            m_layer->paintDevice()->colorSpace()->darken(m_it.rawData(), m_it.rawData(), shade, compensate, compensation, 1);
            return 0;
        }
        Kross::Api::Object::Ptr invertColor(Kross::Api::List::Ptr )
        {
            m_layer->paintDevice()->colorSpace()->invertColor(m_it.rawData(), 1);
            return 0;
        }
        Kross::Api::Object::Ptr next()
        {
            ++m_it;
            return new Kross::Api::Variant(m_it.isDone());
        }
        Kross::Api::Object::Ptr isDone()
        {
            return new Kross::Api::Variant(m_it.isDone());
        }
        Kross::Api::Object::Ptr getChannelUINT8(Kross::Api::List::Ptr, uint channelpos)
        {
            Q_UINT8* data = (Q_UINT8*)(m_it.rawData() + channelpos);
            return new Kross::Api::Variant( * data);
        }
        Kross::Api::Object::Ptr setChannelUINT8(Kross::Api::List::Ptr args, uint channelpos)
        {
            Q_UINT8* data = (Q_UINT8*)(m_it.rawData() + channelpos); //*(uint*)channelpos);
            *data = Kross::Api::Variant::toUInt( args->item(0) );
            return 0;
        }
        Kross::Api::Object::Ptr getChannelUINT16(Kross::Api::List::Ptr, uint channelpos)
        {
            Q_UINT16* data = (Q_UINT16*)(m_it.rawData() + channelpos);
            return new Kross::Api::Variant( * data);
        }
        Kross::Api::Object::Ptr setChannelUINT16(Kross::Api::List::Ptr args, uint channelpos)
        {
            Q_UINT16* data = (Q_UINT16*)(m_it.rawData() + channelpos);
            *data =  Kross::Api::Variant::toUInt( args->item(0) );
            return 0;
        }
        Kross::Api::Object::Ptr getChannelFLOAT(Kross::Api::List::Ptr, uint channelpos)
        {
            float* data = (float*)(m_it.rawData() + channelpos);
            return new Kross::Api::Variant( * data);
        }
        Kross::Api::Object::Ptr setChannelFLOAT(Kross::Api::List::Ptr args, uint channelpos)
        {
            float* data = (float*)(m_it.rawData() + channelpos);
            *data = Kross::Api::Variant::toUInt( args->item(0) );
            return 0;
        }
        Kross::Api::Object::Ptr getPixel(Kross::Api::List::Ptr)
        {
            QValueVector<KisChannelInfo *> channels = m_layer->paintDevice()->colorSpace()->channels();
            QValueList<QVariant> pixel;
            for(QValueVector<KisChannelInfo *>::iterator itC = channels.begin(); itC != channels.end(); itC++)
            {
                KisChannelInfo * ci = *itC;
                Q_UINT8* data = (Q_UINT8*)(m_it.rawData() + ci->pos());
                switch(ci->channelValueType())
                {
                    case KisChannelInfo::UINT8:
                        pixel.push_back( *data);
                        break;
                    case KisChannelInfo::UINT16:
                        pixel.push_back( *((Q_UINT16*) data) );
                        break;
                    case KisChannelInfo::FLOAT32:
                        pixel.push_back( *((float*) data) );
                        break;
                    default:
                        kdDebug() << i18n("An error has occured in %1").arg("getPixel") << endl;
                        kdDebug() << i18n("unsupported data format in scripts") << endl;
                        break;
                }
            }
            return new Kross::Api::Variant( pixel);
        }
        Kross::Api::Object::Ptr setPixel(Kross::Api::List::Ptr args)
        {
            QValueList<QVariant> pixel = Kross::Api::Variant::toList( args->item(0) );
            QValueVector<KisChannelInfo *> channels = m_layer->paintDevice()->colorSpace()->channels();
            uint i = 0;
            for(QValueVector<KisChannelInfo *>::iterator itC = channels.begin(); itC != channels.end(); itC++, i++)
            {
                KisChannelInfo * ci = *itC;
                Q_UINT8* data = (Q_UINT8*)(m_it.rawData() + ci->pos());
                switch(ci->channelValueType())
                {
                    case KisChannelInfo::UINT8:
                        *data = pixel[i].toUInt();
                        break;
                    case KisChannelInfo::UINT16:
                        *((Q_UINT16*) data) = pixel[i].toUInt();
                        break;
                    case KisChannelInfo::FLOAT32:
                        *((float*) data) = pixel[i].toDouble();
                        break;
                    default:
                        kdDebug() << i18n("An error has occured in %1").arg("setPixel") << endl;
                        kdDebug() << i18n("unsupported data format in scripts") << endl;
                        break;
                }
            }
            return 0;
        }
    private:
        _T_It m_it;
        int nchannels;
        KisPaintLayerSP m_layer;
};

}

}

#endif
