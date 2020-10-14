// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'
import * as Card from '../cardSizes'
import * as Background from '../../../../../common/Background'


export default function CardImage (props: { size: string, imageUrl: string }) {
  const [data, setData] = React.useState('');

  React.useEffect(() => {
    Background.send<
    BraveToday.Messages.GetFeedImageDataResponse,
      BraveToday.Messages.GetFeedImageDataPayload
    >(
      Background.MessageTypes.Today.getFeedImageData,
      { url: props.imageUrl }
    ).then(result => {
      setData(result.dataUrl);
    })
  });

  return <Card.Image size={props.size} src={data} />
}