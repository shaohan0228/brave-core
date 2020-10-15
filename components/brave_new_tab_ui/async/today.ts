import * as Background from '../../common/Background'
import AsyncActionHandler from '../../common/AsyncActionHandler'
// import * as Actions from '../actions/today_actions'
import { init } from '../actions/new_tab_actions'
import * as Actions from '../actions/today_actions'

import MessageTypes = Background.MessageTypes.Today
import Messages = BraveToday.Messages

const handler = new AsyncActionHandler()

handler.on(init.getType(), async (store, action) => {
  try {
    const [{feed}, {publishers}] = await Promise.all([
      Background.send<Messages.GetFeedResponse>(MessageTypes.getFeed),
      Background.send<Messages.GetPublishersResponse>(MessageTypes.getPublishers)
    ])
    console.log('got feed', { feed, publishers })
    store.dispatch(Actions.dataReceived({feed, publishers}))
  } catch (e) {
    console.error('error receiving feed', e)
    store.dispatch(Actions.errorGettingDataFromBackground(e))
  }
})


export default handler.middleware