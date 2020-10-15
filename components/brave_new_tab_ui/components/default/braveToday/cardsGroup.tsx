// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'

// Feature Containers
import CardLarge from './cards/_articles/cardArticleLarge'
import CardSmall from './cards/_articles/cardArticleMedium'
import CardBrandedList from './cards/brandedList'
import CardOrderedList from './cards/orderedList'
import CardDeals from './cards/cardDeals'

import * as BraveTodayElement from './default'

enum CardType {
  Headline,
  HeadlinePaired,
  CategoryGroup,
  Deals,
  PublisherGroup,
}

const PageContentOrder = [
  CardType.Headline,
  CardType.Headline,
  CardType.HeadlinePaired,
  CardType.HeadlinePaired,
  CardType.CategoryGroup,
  CardType.Headline,
  CardType.Deals,
  CardType.Headline,
  CardType.HeadlinePaired,
  CardType.PublisherGroup,
  CardType.Headline,
  CardType.HeadlinePaired,
]

const RandomContentOrder = [
  CardType.Headline,
  CardType.HeadlinePaired,
  CardType.Headline,
]

interface Props {
  content: BraveToday.Page
  publishers: BraveToday.Publishers
}

interface State {
  setFocusOnBraveToday: boolean
}

class CardsGroup extends React.PureComponent<Props, State> {
  state = {
    setFocusOnBraveToday: false
  }

  renderCard (cardType: CardType, headlines: BraveToday.Article[]) {
    switch (cardType) {
      case CardType.Headline:
        // TODO: article card should handle any number of articles and
        // adapt accordingly.
        return <CardLarge
                content={headlines.splice(0, 1)}
                publishers={this.props.publishers}
              />
      case CardType.HeadlinePaired:
        // TODO: handle content length < 2
        return <CardSmall
                content={headlines.splice(0, 2)}
                publishers={this.props.publishers}
              />
      case CardType.CategoryGroup:
        if (!this.props.content.itemsByCategory) {
          return null
        }
        return <CardBrandedList
          content={this.props.content.itemsByCategory.items}
          publishers={this.props.publishers}
        />
      case CardType.PublisherGroup:
        if (!this.props.content.itemsByPublisher) {
          return null
        }
        return <CardOrderedList
          content={this.props.content.itemsByPublisher.items}
          publishers={this.props.publishers}
        />
      case CardType.Deals:
        return <CardDeals content={this.props.content.deals} />
    }
    console.error('Asked to render unknown card type', cardType)
    return null
  }

  renderOrder (order: CardType[], articles: BraveToday.Article[]) {
    return (
      <BraveTodayElement.ArticlesGroup>
        { order.map(cardType => this.renderCard(cardType, articles)) }
      </BraveTodayElement.ArticlesGroup>
    )
  }

  render () {
    // Duplicate array so we can splice without affecting state
    const headlines = [...this.props.content.articles]
    const randomHeadlines = [...this.props.content.randomArticles]
    return (
      <>
        {this.renderOrder(PageContentOrder, headlines)}
        {this.renderOrder(RandomContentOrder, randomHeadlines)}
      </>
    )
  }
}

export default CardsGroup
