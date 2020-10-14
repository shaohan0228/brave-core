// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'

// Feature-specific components
import * as Card from './style'
import { Debugger } from '../../default'

interface Props {
  content: (BraveToday.Article | undefined)[]
}

class CardBrandedList extends React.PureComponent<Props, {}> {
  render () {
    const { content }: Props = this.props

    // No content no render®
    if (content.length < 3) {
      return null
    }

    return (
    <Card.BrandedList>
      {
        // If all posts are from the same category, show it
        // @ts-ignore
        (content && content[0] && content[0].category) ===
        // @ts-ignore
        (content && content[1] && content[1].category) &&
        // @ts-ignore
        (content && content[1] && content[1].category) ===
        // @ts-ignore
        (content && content[2] && content[2].category)
        ? (
          <>
              <Debugger>all same CATEGORY</Debugger>
              <Card.Title>{
                // @ts-ignore
                content && content[0] && content[0].category
              }</Card.Title>
            </>
          ) : null
      }
      <Card.List>
        {
          content.map((item, index) => {
            if (item === undefined) {
              return null
            }
            return (
                <Card.ListItem key={`card-ordered-list-key-${index}`}>
                  <a href={item.url}>
                    <Card.Content>
                      <Card.Heading>
                        {item.title}
                      </Card.Heading>
                      <Card.Time>{item.relative_time}</Card.Time>
                    </Card.Content>
                    <Card.Image src={item.img} alt='' />
                  </a>
                </Card.ListItem>
            )
          })
        }
      </Card.List>
      {
        // If all posts are from the same publisher,
        // show their logo
        // @ts-ignore
        (content && content[0] && content[0].publisher_logo) ===
        // @ts-ignore
        (content && content[1] && content[1].publisher_logo) &&
        // @ts-ignore
        (content && content[1] && content[1].publisher_logo) ===
        // @ts-ignore
        (content && content[2] && content[2].publisher_logo)
          ? (
            <Card.PublisherLogo
              // @ts-ignore
              src={content && content[0] && content[0].publisher_logo}
              alt={''}
            />
          ) : null
      }
    </Card.BrandedList>
    )
  }
}

export default CardBrandedList
