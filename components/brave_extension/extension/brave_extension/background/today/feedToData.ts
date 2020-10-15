// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.
import { formatDistanceToNow } from 'date-fns'

function shuffleArray(array: Array<any>) {
  for (let i = array.length - 1; i > 0; i--) {
      const j = Math.floor(Math.random() * (i + 1));
      [array[i], array[j]] = [array[j], array[i]];
  }
}

export function generateRelativeTimeFormat (publishTime: string) {
  if (!publishTime) {
    return
  }
  // TODO(petemill): proper internationalisation syntax
  return formatDistanceToNow(
    new Date(publishTime)
  ) + ' ago'
}

function convertFeedItem(item: BraveToday.FeedItem): BraveToday.FeedItem {
  const publishTime = item.publish_time + ' UTC'
  return {
    ...item,
    img: item.padded_img,
    publish_time: publishTime,
    relative_time: generateRelativeTimeFormat(publishTime),
  }
}

export default async function getBraveTodayData (
  feedContent: BraveToday.FeedItem[],
  enabledPublishers: BraveToday.Publishers
): Promise<BraveToday.Feed | undefined> {
  // No sponsor content yet, wait until we have content spec
  // const sponsors: (BraveToday.Article)[] = []
  const deals: (BraveToday.Deal)[] = []
  let articles: (BraveToday.Article)[] = []

  // Filter to only enabled sources
  feedContent = feedContent.filter(f => !!enabledPublishers[f.publisher_id])
  feedContent = await weightArticles(feedContent)

  for (let feedItem of feedContent) {
    feedItem = convertFeedItem(feedItem)
    switch (feedItem.content_type) {
      // case 'offer':
      //   sponsors.push(feedItem as BraveToday.Article)
      //   break
      case 'product':
        deals.push(feedItem as BraveToday.Deal)
        break
      case 'article':
        articles.push(feedItem as BraveToday.Article)
        break
    }
  }

  // Shuffle top 25 items
  const topArticles = articles.splice(0, 25)
  shuffleArray(topArticles)
  articles = [
    ...topArticles,
    ...articles
  ]

  // Get unique categories present
  const categoryCounts = new Map<string, number>()
  for (const article of articles) {
    if (article.category && article.category !== "Top News") {
      const existingCount = categoryCounts.get(article.category) || 0
      categoryCounts.set(article.category, existingCount + 1)
    }
  }

  // Ordered by # of occurances
  const categoriesByPriority = [
    "Top News",
    ...Array.from(
        categoryCounts.keys()
      ).sort((a, b) => categoryCounts[a] - categoryCounts[b])
  ]

  // Get unique deals categories present
  const dealsCategoryCounts = new Map<string, number>()
  for (const deal of deals) {
    if (deal.offers_category) {
      const existingCount = dealsCategoryCounts.get(deal.offers_category) || 0
      dealsCategoryCounts.set(deal.offers_category, existingCount + 1)
    }
  }
  const dealsCategoriesByPriority = Array.from(dealsCategoryCounts.keys())
    .sort((a, b) => dealsCategoryCounts[a] - dealsCategoryCounts[b])

  // .sponsor,
  // .headline(paired: false),
  // .deals,

  // const firstSponsors = sponsors.splice(0, 1) // Featured sponsor is the first sponsor

  const firstHeadlines = take(articles, isArticleTopNews, 1)
  const firstDeals = deals.splice(0, 3)

  // generate as many pages of content as possible
  const pages: BraveToday.Page[] = []
  let canGenerateAnotherPage = true
  // Sanity check: arbitrary max pages so we don't end up
  // in infinit loop.
  const maxPages = 4000;
  let curPage = 0;
  while (canGenerateAnotherPage) {
    curPage++
    if (curPage > maxPages) break
    const category = categoriesByPriority.shift()
    const dealsCategory = dealsCategoriesByPriority.shift()
    const nextPage = generateNextPage(articles, deals, category, dealsCategory)
    if (!nextPage) {
      canGenerateAnotherPage = false
      continue
    }
    pages.push(nextPage)
  }

  return {
    // featuredSponsor: firstSponsors.length ? firstSponsors[0] : undefined,
    featuredArticle: firstHeadlines.length ? firstHeadlines[0] : undefined,
    featuredDeals: firstDeals,
    pages
  }
}

function isArticleWithImage (article: BraveToday.Article) {
  return !!article.padded_img
}

function isArticleTopNews (article: BraveToday.Article) {
  return article.category === 'Top News'
}

function isArticleWithin48Hours (article: BraveToday.Article) {
  const secondsSincePublish = Math.abs((new Date().getTime() - new Date(article.publish_time).getTime()) / 1000)
  const secondsSince48Hours = 48 * 60 * 60
  return (secondsSincePublish < secondsSince48Hours)
}

function generateNextPage (
  articles: BraveToday.Article[],
  allDeals: BraveToday.Deal[],
  featuredCategory?: string,
  dealsCategory?: string): BraveToday.Page | null {
  // .repeating([.headline(paired: false)], times: 2),
  // .repeating([.headline(paired: true)], times: 2),
  // .categoryGroup (first "Top News" then each category),
  // .headline(paired: false),
  // .deals,
  // .headline(paired: false),
  // .headline(paired: true),
  // .brandedGroup(numbered: true),
  // .group (3 articles),
  // Random items in the last 48 hours:
  //    .headline(paired: false),
  //    .headline(paired: true),
  //    .headline(paired: false),

  // Collect headlines
  const headlines = take(articles, isArticleWithImage, 13)
  if (!headlines.length) {
    return null
  }

  const articlesWithCategory = featuredCategory
    ? take(
        articles,
        a => a.category === featuredCategory,
        3
      )
    : []

  let deals = !dealsCategory
    ? []
    : take(allDeals, d => d.offers_category === dealsCategory, 3)
  if (deals.length < 3) {
    deals.concat(allDeals.splice(0, 3 - deals.length))
  }

  const publisherInfo = generateArticleSourceGroup(articles)

  const randomArticles = take(articles, isArticleWithin48Hours, 4)

  return {
    articles: headlines,
    randomArticles,
    deals,
    itemsByCategory: (featuredCategory && articlesWithCategory.length) ? { categoryName: featuredCategory, items: articlesWithCategory } : undefined,
    itemsByPublisher: publisherInfo ? { name: publisherInfo[0], items: publisherInfo[1] } : undefined,
  }
}

function generateArticleSourceGroup (articles: BraveToday.Article[]): [string, BraveToday.Article[]] | undefined {
  const firstArticleWithSource = articles.find(a => !!a.publisher_id)
  if (firstArticleWithSource) {
    return [
      firstArticleWithSource.publisher_id,
      take(
        articles,
        a => a.publisher_id === firstArticleWithSource.publisher_id,
        3
      )
    ]
  }
  return undefined
}

function take<T>(items: T[], matching: (item: T) => boolean, count?: number, random: boolean = false): T[] {
  let indicesToTake: number[] = []
  for (const [i, item] of items.entries())  {
    const shouldTake = matching(item)
    if (!shouldTake) {
      continue
    }
    // Take item
    indicesToTake.push(i)
    // Stop, if we're limiting the count
    if (!random && count && indicesToTake.length === count) {
      break
    }
  }
  // Randomize
  if (random) {
    shuffleArray(indicesToTake)
    if (count && indicesToTake.length > count) {
      indicesToTake = indicesToTake.slice(0, count)
    }
  }
  // Get items and remove from array
  indicesToTake = indicesToTake.reverse()
  const takenItems: T[] = []
  for (const i of indicesToTake) {
    takenItems.push(...items.splice(i, 1))
  }
  return takenItems.reverse()
}

function domainFromUrlString(urlString: string): string {
  return new window.URL(urlString).host
}

async function weightArticles(articles: BraveToday.FeedItem[]): Promise<BraveToday.FeedItem[]> {
  // hosts from latest max-200 history items
  const historyItems: chrome.history.HistoryItem[] = await new Promise(resolve => chrome.history.search({ text: '', maxResults: 200 }, resolve))
  const historyHosts: string[] = historyItems.map(h => h.url).filter(i => i).map((u: string) => domainFromUrlString(u))
  for (const article of articles) {
    let score = article.score
    const hasDomainHistoryMatch = historyHosts.includes(domainFromUrlString(article.url))
    if (hasDomainHistoryMatch) {
      score -= 5
    }
    article.points = score
  }
  articles = articles.sort((a, b) => (a.points || 1000) - (b.points || 1000))
  return articles
}
