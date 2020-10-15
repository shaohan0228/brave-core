declare namespace BraveToday {

  // Messages
  namespace Messages {
    export type GetFeedResponse = {
      feed: BraveToday.Feed | undefined
    }
    export type GetPublishersResponse = {
      publishers: BraveToday.Publishers | undefined
    }
    export type GetImageDataPayload = {
      url: string
    }
    export type GetImageDataResponse = {
      dataUrl: string
    }
  }

  export type FeedItem = (Article | Deal)

  export interface Feed {
    featuredSponsor?: Article
    featuredArticle?: Article
    featuredDeals?: Deal[]
    pages: Page[]
  }

  export interface Page {
    articles: Article[] // 13
    randomArticles: Article[] // 4
    itemsByCategory?: {
      categoryName: string
      items: Article[] // 3
    }
    itemsByPublisher?: {
      name: string,
      items: Article[] // 3
    }
    deals: Deal[] // 3

  }

  export interface ScrollingList {
    sponsors: Article[]
    deals: Deal[]
    articles: Article[]
  }

  type BaseFeedItem = {
    content_type: 'article' | 'product'
    category: string // 'Tech', 'Business', 'Top News', 'Crypto', 'Cars', 'Culture', 'Fashion', 'Sports', 'Entertainment'
    publish_time: string // UTC "2020-04-17 19:21:10"
    title: string // "14 Truly Incredible Catfish Makeup Transformations From TikTok"
    description: string // "# Makeup skill level: Expert.↵↵![](https://img.buzzfeed.com/buzzfeed-↵static/static/2020-04/6/20/enhanced/a3cd932e6db6/original-567-1586204318-9.jpg?crop=1244:829;0,0)↵↵* * *↵↵[View Entire Post ›](https://www.buzzfeed.com/kristatorres/13-truly-↵incredible-catfish-makeup-transformations)↵↵"
    url: string // "https://www.buzzfeed.com/kristatorres/13-truly-incredible-catfish-makeup-transformations"
    url_hash: string // '0e57ac...'
    padded_img: string
    img: string // '',
    publisher_id: string // 'afd9...'
    publisher_name: string
    score: number
    relative_time?: string
    // Custom for this application. Does not come from source
    points?: number
  }

  export type Article = BaseFeedItem & {
    content_type: 'article'
  }

  export type Deal = BaseFeedItem & {
    content_type: 'product'
    offers_category: string // 'Companion Products
  }

  export type Publisher = {
    publisher_id: string,
    publisher_name: string,
    category: string,
    publisher_logo_padded: string,
    enabled: boolean
    user_enabled: boolean | null
  }

  export type Publishers = {
    [publisher_id: string]: Publisher
  }
}
