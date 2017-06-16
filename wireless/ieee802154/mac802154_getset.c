/****************************************************************************
 * wireless/ieee802154/mac802154_getset.c
 *
 *   Copyright (C) 2016 Sebastien Lorquet. All rights reserved.
 *   Copyright (C) 2017 Gregory Nutt. All rights reserved.
 *   Copyright (C) 2017 Verge Inc. All rights reserved.
 *
 *   Author: Sebastien Lorquet <sebastien@lorquet.fr>
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *   Author: Anthony Merlino <anthony@vergeaero.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>
#include <string.h>

#include "mac802154.h"
#include "mac802154_internal.h"

#include <nuttx/wireless/ieee802154/ieee802154_mac.h>

/****************************************************************************
 * Public MAC Functions
 ****************************************************************************/

/****************************************************************************
 * Name: mac802154_req_get
 *
 * Description:
 *   The MLME-GET.request primitive requests information about a given PIB
 *   attribute.
 *
 *   NOTE: The standard specifies that the attribute value should be returned
 *   via the asynchronous MLME-GET.confirm primitve.  However, in our
 *   implementation, we synchronously return the value immediately.Therefore, we
 *   merge the functionality of the MLME-GET.request and MLME-GET.confirm
 *   primitives together.
 *
 ****************************************************************************/

int mac802154_req_get(MACHANDLE mac, enum ieee802154_attr_e attr,
                      FAR union ieee802154_attr_u *attrval)
{
  FAR struct ieee802154_privmac_s *priv =
    (FAR struct ieee802154_privmac_s *)mac;
  int ret = IEEE802154_STATUS_SUCCESS;

  switch (attr)
    {
      case IEEE802154_ATTR_MAC_PANID:
        attrval->mac.panid = priv->addr.panid;
        break;
      case IEEE802154_ATTR_MAC_SHORT_ADDRESS:
        attrval->mac.saddr = priv->addr.saddr;
        break;
      case IEEE802154_ATTR_MAC_EXTENDED_ADDR:
        memcpy(&attrval->mac.eaddr[0], &priv->addr.eaddr[0], IEEE802154_EADDR_LEN);
        break;
      case IEEE802154_ATTR_MAC_DEVMODE:
        attrval->mac.devmode = priv->devmode;
        break;
      default:
        /* The attribute may be handled soley in the radio driver, so pass
         * it along.
         */

        ret = priv->radio->set_attr(priv->radio, attr, attrval);
        break;
    }

  return ret;
}

/****************************************************************************
 * Name: mac802154_req_set
 *
 * Description:
 *   The MLME-SET.request primitive attempts to write the given value to the
 *   indicated MAC PIB attribute.
 *
 *   NOTE: The standard specifies that confirmation should be indicated via
 *   the asynchronous MLME-SET.confirm primitve.  However, in our implementation
 *   we synchronously return the status from the request. Therefore, we do merge
 *   the functionality of the MLME-SET.request and MLME-SET.confirm primitives
 *   together.
 *
 ****************************************************************************/

int mac802154_req_set(MACHANDLE mac, enum ieee802154_attr_e attr,
                      FAR const union ieee802154_attr_u *attrval)
{
  FAR struct ieee802154_privmac_s *priv =
    (FAR struct ieee802154_privmac_s *)mac;
  int ret;

  switch (attr)
    {
      case IEEE802154_ATTR_MAC_PANID:
        {
          priv->addr.panid = attrval->mac.panid;

          /* Tell the radio about the attribute */

          priv->radio->set_attr(priv->radio, attr, attrval);

          ret = IEEE802154_STATUS_SUCCESS;
        }
        break;
      case IEEE802154_ATTR_MAC_SHORT_ADDRESS:
        {
          priv->addr.saddr = attrval->mac.saddr;

          /* Tell the radio about the attribute */

          priv->radio->set_attr(priv->radio, attr, attrval);

          ret = IEEE802154_STATUS_SUCCESS;
        }
        break;
      case IEEE802154_ATTR_MAC_EXTENDED_ADDR:
        {
          /* Set the MAC copy of the address in the table */

          memcpy(&priv->addr.eaddr[0], &attrval->mac.eaddr[0],
                 IEEE802154_EADDR_LEN);

          /* Tell the radio about the attribute */

          priv->radio->set_attr(priv->radio, attr, attrval);

          ret = IEEE802154_STATUS_SUCCESS;
        }
        break;
      default:
        {
          /* The attribute may be handled soley in the radio driver, so pass
           * it along.
           */

          ret = priv->radio->set_attr(priv->radio, attr, attrval);
        }
        break;
    }
  return ret;
}