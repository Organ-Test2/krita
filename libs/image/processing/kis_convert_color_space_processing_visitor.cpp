/*
 *  Copyright (c) 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_convert_color_space_processing_visitor.h"

#include "kis_external_layer_iface.h"

#include "kis_paint_device.h"
#include "kis_transaction.h"
#include "kis_undo_adapter.h"
#include "kis_transform_mask.h"
#include "lazybrush/kis_colorize_mask.h"

#include <KoColorConversionTransformation.h>
#include "kis_projection_leaf.h"
#include "kis_paint_layer.h"
#include "kis_time_range.h"


KisConvertColorSpaceProcessingVisitor::KisConvertColorSpaceProcessingVisitor(const KoColorSpace *srcColorSpace,
                                                                             const KoColorSpace *dstColorSpace,
                                                                             KoColorConversionTransformation::Intent renderingIntent,
                                                                             KoColorConversionTransformation::ConversionFlags conversionFlags)
    : m_srcColorSpace(srcColorSpace)
    , m_dstColorSpace(dstColorSpace)
    , m_renderingIntent(renderingIntent)
    , m_conversionFlags(conversionFlags)

{
}


void KisConvertColorSpaceProcessingVisitor::visitExternalLayer(KisExternalLayer *layer, KisUndoAdapter *undoAdapter)
{
    Q_UNUSED(layer);
    Q_UNUSED(undoAdapter);
}

void KisConvertColorSpaceProcessingVisitor::visitNodeWithPaintDevice(KisNode *node, KisUndoAdapter *undoAdapter)
{    if (!node->projectionLeaf()->isLayer()) return;
    if (*m_dstColorSpace == *node->colorSpace()) return;

    bool alphaLock = false;

    KisLayer *layer = dynamic_cast<KisLayer*>(node);

    KisPaintLayer *paintLayer = 0;

    // TODO: channel flags reset should be undoable
    if (m_srcColorSpace->colorModelId() != m_dstColorSpace->colorModelId()) {
        layer->setChannelFlags(QBitArray());
        if ((paintLayer = dynamic_cast<KisPaintLayer*>(layer))) {
            alphaLock = paintLayer->alphaLocked();
            paintLayer->setChannelLockFlags(QBitArray());
        }
    }

    KUndo2Command *parentConversionCommand = new KUndo2Command();

    if (layer->original()) {
        layer->original()->convertTo(m_dstColorSpace, m_renderingIntent, m_conversionFlags, parentConversionCommand);
    }

    if (layer->paintDevice()) {
        layer->paintDevice()->convertTo(m_dstColorSpace, m_renderingIntent, m_conversionFlags, parentConversionCommand);
    }

    if (layer->projection()) {
        layer->projection()->convertTo(m_dstColorSpace, m_renderingIntent, m_conversionFlags, parentConversionCommand);
    }

    undoAdapter->addCommand(parentConversionCommand);

    if (paintLayer) {
        paintLayer->setAlphaLocked(alphaLock);
    }
    layer->invalidateFrames(KisTimeRange::infinite(0), layer->extent());
}

void KisConvertColorSpaceProcessingVisitor::visit(KisTransformMask *node, KisUndoAdapter *undoAdapter)
{
    node->threadSafeForceStaticImageUpdate();
    KisSimpleProcessingVisitor::visit(node, undoAdapter);
}

void KisConvertColorSpaceProcessingVisitor::visitColorizeMask(KisColorizeMask *node, KisUndoAdapter *undoAdapter)
{
    undoAdapter->addCommand(node->setColorSpace(m_dstColorSpace, m_renderingIntent, m_conversionFlags));
    node->invalidateFrames(KisTimeRange::infinite(0), node->extent());
}
